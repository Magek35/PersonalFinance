#include "FinanceManager.h"

// ===== Реализации Models (из Models.h) =====

// Account
Account::Account(const std::string& n)
    : name_(n), balance_(0.0) {
}

Account::~Account() {}

const std::string& Account::getName() const { return name_; }
double Account::getBalance() const { return balance_; }

Account& Account::setBalance(double newBalance) {
    balance_ = newBalance;
    return *this;
}

void Account::addMoney(double amount) {
    if (amount > 0.0) balance_ += amount;
}

bool Account::spendMoney(double amount) {
    if (amount > 0.0 && balance_ >= amount) {
        balance_ -= amount;
        return true;
    }
    return false;
}

// Wallet
Wallet::Wallet(const std::string& n) : Account(n) {}

// Card
Card::Card(const std::string& n, double limit)
    : Account(n), creditLimit_(limit) {
}

bool Card::spendMoney(double amount) {
    if (amount > 0.0 && balance_ + creditLimit_ >= amount) {
        balance_ -= amount;
        return true;
    }
    return false;
}

// Category
Category::Category(const std::string& n) : name_(n) {}
const std::string& Category::getName() const { return name_; }

// Expense
Expense::Expense()
    : description_(), amount_(0.0),
    category_(nullptr), date_(), account_(nullptr) {
}

Expense::Expense(const std::string& desc, double amt,
    const Category* cat, const std::string& dt,
    Account* acc)
    : description_(desc), amount_(amt),
    category_(cat), date_(dt), account_(acc) {
}

const std::string& Expense::getDescription() const { return description_; }
double Expense::getAmount() const { return amount_; }
const Category& Expense::getCategory() const { return *category_; }
const std::string& Expense::getDate() const { return date_; }
Account* Expense::getAccount() const { return account_; }

// ===== Утилиты дат для FinanceManager =====

// проверка YYYY-MM-DD
bool FinanceManager::isValidDateFormat(const std::string& d) {
    if (d.size() != 10) return false;
    if (d[4] != '-' || d[7] != '-') return false;
    for (size_t i : {0u, 1u, 2u, 3u, 5u, 6u, 8u, 9u}) {
        if (!std::isdigit(static_cast<unsigned char>(d[i])))
            return false;
    }
    return true;
}

bool FinanceManager::isDateInRange(const std::string& d,
    const std::string& minD,
    const std::string& maxD) {
    // для формата YYYY-MM-DD лексикографическое сравнение совпадает с хронологическим
    return d >= minD && d <= maxD;
}

std::string FinanceManager::getToday() {
    std::time_t t = std::time(NULL);
    std::tm tm{};
#if defined(_WIN32)
    localtime_s(&tm, &t);
#else
    tm = *std::localtime(&t);
#endif
    char buf[11];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d", &tm);
    return std::string(buf);
}

bool FinanceManager::isSameDay(const std::string& d,
    const std::string& today) {
    return d == today;
}

bool FinanceManager::isInLastNDays(const std::string& d,
    const std::string& today,
    int days) {
    std::tm tm_today{}, tm_d{};
    char dash;
    std::istringstream iss_today(today);
    std::istringstream iss_d(d);

    iss_today >> tm_today.tm_year >> dash >> tm_today.tm_mon >> dash >> tm_today.tm_mday;
    iss_d >> tm_d.tm_year >> dash >> tm_d.tm_mon >> dash >> tm_d.tm_mday;
    if (iss_today.fail() || iss_d.fail()) return false;

    tm_today.tm_year -= 1900;
    tm_today.tm_mon -= 1;
    tm_d.tm_year -= 1900;
    tm_d.tm_mon -= 1;

    std::time_t tt_today = std::mktime(&tm_today);
    std::time_t tt_d = std::mktime(&tm_d);
    if (tt_today == -1 || tt_d == -1) return false;

    double diffSec = std::difftime(tt_today, tt_d);
    double diffDays = diffSec / (60 * 60 * 24);

    return diffDays >= 0.0 && diffDays < days;
}

bool FinanceManager::isSameMonth(const std::string& d,
    const std::string& today) {
    if (d.size() < 7 || today.size() < 7) return false;
    // YYYY-MM одинаковые
    return d.substr(0, 7) == today.substr(0, 7);
}

// ===== FinanceManager =====

FinanceManager::FinanceManager()
    : minDate_("1900-01-01"),
    maxDate_("2099-12-31") {
}

void FinanceManager::setDateBounds(const std::string& minDate,
    const std::string& maxDate) {
    minDate_ = minDate;
    maxDate_ = maxDate;
}

Account* FinanceManager::findAccount(const std::string& name) const {
    std::string low = toLowerStr(name);
    std::vector<Account*>::const_iterator it =
        std::find_if(accounts_.begin(), accounts_.end(),
            [low](const Account* acc) {
                return toLowerStr(acc->getName()) == low;
            });
    return (it != accounts_.end()) ? *it : NULL;
}

void FinanceManager::addAccount(Account* acc) {
    if (acc != NULL && accounts_.size() < MAX_ACCOUNTS)
        accounts_.push_back(acc);
}

void FinanceManager::addCategory(const std::string& name) {
    std::string low = toLowerStr(name);
    if (categories_.find(low) == categories_.end())
        categories_.insert(std::make_pair(low, Category(name)));
}

void FinanceManager::addMoney(const std::string& accName, double amount) {
    Account* acc = findAccount(accName);
    if (acc != NULL) {
        acc->addMoney(amount);
        std::cout << "Пополнено на " << amount
            << " на счёт [" << acc->getName() << "]\n";
    }
    else {
        std::cout << "Счёт '" << accName << "' не найден!\n";
    }
}

void FinanceManager::addExpense(const std::string& accName,
    const std::string& catName,
    const std::string& desc,
    double amount,
    const std::string& date) {
    if (!isValidDateFormat(date) ||
        !isDateInRange(date, minDate_, maxDate_)) {
        std::cout << "Ошибка: неверная дата или вне допустимого диапазона!\n";
        return;
    }

    Account* acc = findAccount(accName);
    if (acc == NULL || !acc->spendMoney(amount)) {
        std::cout << "Ошибка: счёт не найден или недостаточно средств!\n";
        return;
    }

    std::string lowCat = toLowerStr(catName);
    if (categories_.find(lowCat) == categories_.end())
        categories_.insert(std::make_pair(lowCat, Category(catName)));

    Category& cat = categories_[lowCat];
    Expense e(desc, amount, &cat, date, acc);
    expenses_.push_back(e);
    std::cout << "Затрата '" << desc << "' добавлена!\n";
}

void FinanceManager::generateReport(const std::string& period,
    const std::string& filename) {
    std::map<std::string, double> sums;

    std::string today = getToday();

    for (size_t i = 0; i < expenses_.size(); ++i) {
        const Expense& e = expenses_[i];
        const std::string& d = e.getDate();

        bool use = false;
        if (period == "day") {
            use = isSameDay(d, today);          // только за сегодня
        }
        else if (period == "week") {
            use = isInLastNDays(d, today, 7);   // последние 7 дней
        }
        else if (period == "month") {
            use = isSameMonth(d, today);        // за текущий месяц
        }
        else {
            use = true;                         // неизвестный период — берём всё
        }

        if (!use) continue;

        sums[e.getCategory().getName()] += e.getAmount();
    }

    std::ofstream file(filename.c_str());
    if (!file.is_open()) {
        std::cout << "Не удалось открыть файл отчёта!\n";
        return;
    }

    file << "ОТЧЁТ ПО " << period << "\n\n";
    file << "Категория(ключ)\tСумма\n";

    double total = std::accumulate(
        sums.begin(), sums.end(), 0.0,
        [](double res, const std::pair<const std::string, double>& p) {
            return res + p.second;
        });

    for (std::map<std::string, double>::const_iterator it = sums.begin();
        it != sums.end(); ++it) {
        file << it->first << "\t\t"
            << std::fixed << std::setprecision(2) << it->second << "\n";
    }

    file << "\nИТОГО: " << std::fixed << std::setprecision(2) << total << "\n";
    file.close();
    std::cout << "Отчёт сохранён в " << filename << "\n";
}

void FinanceManager::generateTop(const std::string& period,
    const std::string& filename) {
    std::map<std::string, double> sums;
    for (size_t i = 0; i < expenses_.size(); ++i) {
        const Expense& e = expenses_[i];
        sums[e.getCategory().getName()] += e.getAmount();
    }

    std::vector<std::pair<std::string, double> > top;
    top.reserve(sums.size());

    std::transform(
        sums.begin(), sums.end(), std::back_inserter(top),
        [](const std::pair<const std::string, double>& p) {
            return std::make_pair(p.first, p.second);
        });

    std::sort(
        top.begin(), top.end(),
        [](const std::pair<std::string, double>& a,
            const std::pair<std::string, double>& b) {
                return a.second > b.second;
        });

    if (top.size() > 3) top.resize(3);

    std::ofstream file(filename.c_str());
    if (!file.is_open()) {
        std::cout << "Не удалось открыть файл топа!\n";
        return;
    }

    file << "ТОП-3 " << period << "\n";
    for (size_t i = 0; i < top.size(); ++i) {
        file << (i + 1) << ". " << top[i].first << ": "
            << std::fixed << std::setprecision(2) << top[i].second << "\n";
    }
    file.close();
    std::cout << "Топ сохранён в " << filename << "\n";
}

void FinanceManager::saveToFile(const std::string& filename) {
    std::ofstream file(filename.c_str());
    if (!file.is_open()) {
        std::cout << "Не удалось открыть файл для сохранения!\n";
        return;
    }

    file << "date,description,amount,category,account\n";
    for (size_t i = 0; i < expenses_.size(); ++i) {
        const Expense& e = expenses_[i];
        file << e.getDate() << ","
            << e.getDescription() << ","
            << std::fixed << std::setprecision(2) << e.getAmount() << ","
            << e.getCategory().getName() << ","
            << e.getAccount()->getName() << "\n";
    }
    file.close();
    std::cout << "Данные сохранены в " << filename << "\n";
}

void FinanceManager::loadFromFile(const std::string& filename) {
    std::ifstream file(filename.c_str());
    if (!file.is_open()) {
        std::cout << "Файл не найден!\n";
        return;
    }

    std::string line;
    std::getline(file, line); // пропускаем строку заголовка

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string date, desc, cat, accName, amountStr;
        double amount = 0.0;

        std::getline(ss, date, ',');
        std::getline(ss, desc, ',');
        std::getline(ss, amountStr, ',');
        std::getline(ss, cat, ',');
        std::getline(ss, accName, ',');

        std::stringstream a(amountStr);
        a >> amount;

        if (!isValidDateFormat(date) ||
            !isDateInRange(date, minDate_, maxDate_)) {
            std::cout << "Пропущена запись с некорректной датой: " << date << "\n";
            continue;
        }

        std::string lowCat = toLowerStr(cat);
        if (categories_.find(lowCat) == categories_.end())
            categories_.insert(std::make_pair(lowCat, Category(cat)));

        Category& category = categories_[lowCat];

        Account* acc = findAccount(accName);
        if (acc == NULL) {
            acc = new Account(accName);
            addAccount(acc);
        }

        Expense e(desc, amount, &category, date, acc);
        expenses_.push_back(e);
    }

    file.close();
    std::cout << "Данные загружены из " << filename << "\n";
}

const std::vector<Account*>& FinanceManager::getAccounts() const {
    return accounts_;
}