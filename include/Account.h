#pragma once

#include "Transaction.h" //нужен класс Transaction, потому что в Account есть история транзакций
#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <optional>

class Client;

// enable_shared_from_this нужен чтобы объект мог безопасно создать shared_ptr на самого себя
// объекты Account в проекте создаются и хранятся через shared ptr
class Account : public std::enable_shared_from_this<Account> {
protected:

   //доступно внутри самого класса, доступно в дочерних классах, недоступно напрямую извне

    std::string accountNumber; // Уникальный номер банковского счёта.
    double balance; // Текущее количество денег на счёте.
    std::weak_ptr<Client> owner;
    // Ссылка на владельца счёта. Client владеет своими счетами через shared_ptr<Account>
    //а Account знает своего клиента, но не владеет им, поэтому хранит weak_ptr<Client>
    //weak_ptr позволяет ссылаться на объект но не увеличивать счётчик владения, избегаем цикла

    std::chrono::system_clock::time_point openDate; //Момент времени, когда счёт был открыт.
    bool isActive;
    std::vector<Transaction> transactions;


   //Получить доступ к балансу для операций перевода
   // в ретерне сылка на баланс
protected:
    double& getBalanceRef() { return balance; }


public:
    Account(const std::string& accountNumber, //Создаёт банковский счёт.
            std::shared_ptr<Client> owner,
            double initialBalance = 0.0);

    virtual ~Account() = default; //вирт.деструктор, при удалении через указатель на базовый класс должен вызваться деструктор потомка

    std::string getAccountNumber() const { return accountNumber; } // геттер номера счета, конст - не меняем объект

    double getBalance() const { return balance; } // геттер баланса

    std::shared_ptr<Client> getOwner() const; // weak_ptr нельзя использовать напрямую, метод превращает его в shared_ptr через lock()

    std::chrono::system_clock::time_point getOpenDate() const { return openDate; } // дата открытия

    bool getIsActive() const { return isActive; } //проверка активности счета

    const std::vector<Transaction>& getTransactionHistory() const { return transactions; } // возвращает историю транзакций

    //Возвращает список транзакций за период от startDate до endDate
    std::vector<Transaction> getTransactionsByDate(
        const std::chrono::system_clock::time_point& startDate,
        const std::chrono::system_clock::time_point& endDate) const;

    //Пополняет/уменьшает счёт. Потомки могут переопределить логику. Обычный счёт пополняется, спецсчёт может иметь ограничения
    virtual bool deposit(double amount, const std::string& description = "");

    virtual bool withdraw(double amount, const std::string& description = "");

    virtual bool transfer(std::shared_ptr<Account> to, double amount, const std::string& description = "");

    void activate();

    void deactivate();

    virtual double calculateInterest() const = 0; //вирт.функция начисления процентов

    virtual std::string getAccountType() const = 0; //вирт.функция получения типа счета

    //применяем начисл.проценты к счету
    virtual bool applyInterestPayment(double amount, const std::string& description = "Interest payment");

protected:
    void addTransaction(const Transaction& transaction); //ведение журнала транзакций

    bool validateOperation(double amount) const;  //можно ли выполнить операцию на указанную сумму
};