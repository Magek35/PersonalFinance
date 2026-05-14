#include "FinanceManager.h"

class ConsoleUI {
public:
    void run() {
        std::setlocale(LC_ALL, "Russian");

        FinanceManager fm;
        fm.addAccount(new Wallet("nalichnye"));
        fm.addAccount(new Card("debetovaya"));
        fm.addAccount(new Card("kreditnaya", 50000.0));
        fm.addCategory("eda");
        fm.addCategory("transport");
        fm.addCategory("razvlecheniya");
        fm.addCategory("zdorove");

        // Пример границ дат (можешь поменять при необходимости)
        fm.setDateBounds("2000-01-01", "2100-12-31");

        int choice;
        std::string accName, catName, desc, date;
        double amount;

        while (true) {
            printMenu();
            std::cin >> choice;
            if (std::cin.fail()) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }
            if (choice == 0) break;

            switch (choice) {
            case 1:
                std::cout << "Счёт (nalichnye / debetovaya / kreditnaya): ";
                std::cin >> accName;
                std::cout << "Сумма: ";
                std::cin >> amount;
                fm.addMoney(accName, amount);
                break;
            case 2:
                std::cout << "Счёт (nalichnye / debetovaya / kreditnaya): ";
                std::cin >> accName;
                std::cout << "Категория (eda / transport / razvlecheniya / zdorove): ";
                std::cin >> catName;
                std::cout << "Описание (можно по-русски): ";
                std::cin.ignore();
                std::getline(std::cin, desc);
                std::cout << "Сумма: ";
                std::cin >> amount;
                std::cout << "Дата (YYYY-MM-DD): ";
                std::cin >> date;
                fm.addExpense(accName, catName, desc, amount, date);
                break;
            case 3:
                fm.generateReport("day", "report_day.txt");
                break;
            case 4:
                fm.generateReport("week", "report_week.txt");
                break;
            case 5:
                fm.generateReport("month", "report_month.txt");
                break;
            case 6:
                fm.generateTop("week", "top_week.txt");
                break;
            case 7:
                fm.generateTop("month", "top_month.txt");
                break;
            case 8:
                fm.saveToFile("finances.csv");
                break;
            case 9:
                fm.loadFromFile("finances.csv");
                break;
            default:
                break;
            }
        }

        const std::vector<Account*>& accs = fm.getAccounts();
        for (size_t i = 0; i < accs.size(); ++i) delete accs[i];

        std::cout << "До свидания!\n";
    }

private:
    void printMenu() const {
        std::cout << "\n ФИНАНСЫ \n";
        std::cout << "1. Пополнить счёт\n";
        std::cout << "2. Добавить затрату\n";
        std::cout << "3. Отчёт по дню\n";
        std::cout << "4. Отчёт по неделе\n";
        std::cout << "5. Отчёт по месяцу\n";
        std::cout << "6. Топ-3 (неделя)\n";
        std::cout << "7. Топ-3 (месяц)\n";
        std::cout << "8. Сохранить данные\n";
        std::cout << "9. Загрузить данные\n";
        std::cout << "0. Выход\n>> ";
    }
};

int main() {
    ConsoleUI ui;
    ui.run();
    return 0;
}
