#pragma once
#include <string>

// Базовый класс для любых счетов
class Account {
protected:
    std::string name_;    // имя счёта
    double      balance_; // текущий баланс
public:
    explicit Account(const std::string& n);
    virtual ~Account();

    const std::string& getName() const;
    double             getBalance() const;

    Account& setBalance(double newBalance);

    virtual void addMoney(double amount);
    virtual bool spendMoney(double amount);
};

// Кошелёк (наличные)
class Wallet : public Account {
public:
    explicit Wallet(const std::string& n);
};

// Банковская карта с кредитным лимитом
class Card : public Account {
    double creditLimit_; // максимально доступный "минус" сверх баланса
public:
    Card(const std::string& n, double limit = 0.0);
    bool spendMoney(double amount) override;
};

// Категория расхода
class Category {
    std::string name_;
public:
    explicit Category(const std::string& n = "unknown");
    const std::string& getName() const;
};

// Конкретная затрата
class Expense {
    std::string        description_; // описание расхода
    double             amount_;      // сумма расхода
    const Category* category_;    // указатель на категорию
    std::string        date_;        // строка даты (ожидаем YYYY-MM-DD)
    Account* account_;     // указатель на счёт
public:
    Expense();
    Expense(const std::string& desc, double amt,
        const Category* cat, const std::string& dt,
        Account* acc);

    const std::string& getDescription() const;
    double             getAmount()      const;
    const Category& getCategory()    const;
    const std::string& getDate()        const;
    Account* getAccount()     const;
};
