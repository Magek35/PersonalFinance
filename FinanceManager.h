#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <iomanip>
#include <sstream>
#include <limits>
#include <locale>
#include <cctype>
#include <ctime>   // дл€ работы с датами

#include "Models.h"

// приводит строку к нижнему регистру
inline std::string toLowerStr(const std::string& s) {
    std::string result = s;
    for (size_t i = 0; i < result.size(); ++i)
        result[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(result[i])));
    return result;
}

// ћенеджер финансов: счета, категории, расходы, файлы
class FinanceManager {
public:
    static const std::size_t MAX_ACCOUNTS = 200; // ограничение на количество счетов

    FinanceManager();

    void addAccount(Account* acc);
    void addCategory(const std::string& name);

    void addMoney(const std::string& accName, double amount);

    void addExpense(const std::string& accName, const std::string& catName,
        const std::string& desc, double amount,
        const std::string& date);

    void generateReport(const std::string& period,
        const std::string& filename);
    void generateTop(const std::string& period,
        const std::string& filename);

    void saveToFile(const std::string& filename);
    void loadFromFile(const std::string& filename);

    const std::vector<Account*>& getAccounts() const;

    // границы допустимых дат (включительно), формат YYYY-MM-DD
    void setDateBounds(const std::string& minDate,
        const std::string& maxDate);

private:
    std::vector<Expense>  expenses_;   // список всех расходов
    std::vector<Account*> accounts_;   // все счета
    std::map<std::string, Category> categories_; // категории по ключу в нижнем регистре

    std::string minDate_; // нижн€€ граница дат
    std::string maxDate_; // верхн€€ граница дат

    Account* findAccount(const std::string& name) const;

    static bool isValidDateFormat(const std::string& d);
    static bool isDateInRange(const std::string& d,
        const std::string& minD,
        const std::string& maxD);

    // служебные функции дл€ дат
    static std::string getToday(); // текуща€ дата YYYY-MM-DD
    static bool isSameDay(const std::string& d,
        const std::string& today);
    static bool isInLastNDays(const std::string& d,
        const std::string& today,
        int days);
    static bool isSameMonth(const std::string& d,
        const std::string& today);
};