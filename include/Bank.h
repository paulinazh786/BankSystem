#pragma once

#include "Client.h"
#include "Employee.h"
#include "Account.h"
#include "Transaction.h"
#include "DebitAccount.h"
#include "CreditAccount.h"
#include "DepositAccount.h"
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <optional>
#include <chrono>

class Bank {
private:
    /// constexpr — значение задаётся при компиляции, не хранится отдельно в объекте.
    static constexpr std::size_t MIN_BANK_NAME_LENGTH = 1;
    std::string name;
    std::vector<std::shared_ptr<Client>> clients; //умный указатель на объект клиента.банк хранит набор объектов клиентов
    std::vector<std::shared_ptr<Employee>> employees; //аналогично
    std::vector<std::shared_ptr<Account>> accounts; //аналогично Причём тут используется базовый класс Account, внутри могут лежать разные типы счетов. банк работает с ними как с общим типом “счёт”.
    std::vector<Transaction> transactions; //История всех транзакций.
    double totalAssets;

    static int transactionIdCounter; //Общий счётчик для ID транзакций. static - общий для класса



public:
    explicit Bank(const std::string& bankName); //конструктор Создаёт объект банка с названием. Защита от неявных преобразований.

    std::string getName() const { return name; } //геттеры здесь и далее

    const std::vector<std::shared_ptr<Client>>& getClients() const { return clients; }

    const std::vector<std::shared_ptr<Employee>>& getEmployees() const { return employees; }

    const std::vector<std::shared_ptr<Account>>& getAccounts() const { return accounts; }

    const std::vector<Transaction>& getTransactions() const { return transactions; }

    double getTotalAssets() const { return totalAssets; }

    void addClient(std::shared_ptr<Client> client); //метод добавляет нового клиента в банк.

    bool removeClient(int clientId); //метод удаляет нового клиента из банка.

    std::optional<std::shared_ptr<Client>> findClientById(int id) const; //ищем клиента по айди. optional - если клиент найден, вернётся объект клиента, если нет, то пустое значение.

    std::optional<std::shared_ptr<Client>> findClientByPassport(const std::string& passport) const; //поиск по паспорту

    std::vector<std::shared_ptr<Client>> findClientsByName(const std::string& name) const; //поиск по имени

    void hireEmployee(std::shared_ptr<Employee> employee); //нанимаем сотрудника, т е добавляем в список сотрудников банка

    bool fireEmployee(int employeeId); //увольняем по айди

    std::optional<std::shared_ptr<Employee>> findEmployeeById(int id) const; //ищем сотрудника по айди, если не находем, то возвращаем пустое значение

    void openAccount(std::shared_ptr<Account> account); //открываем - добавляем - новый счет в банк

    bool closeAccount(const std::string& accountNumber); //закрываем счет

    std::optional<std::shared_ptr<Account>> findAccountByNumber(const std::string& accountNumber) const; //ищем счет по номеру, если не находим, то возвращаем пустое знач (опшнл)

    std::vector<std::shared_ptr<Account>> getAccountsByClient(int clientId) const; //получаем счет от конкретного клиента

    bool performTransaction(const Transaction& transaction); //выполняем транзакцию

    //переводим деньги с одного счета на другой
    bool transferMoney(const std::string& fromAccount,
                      const std::string& toAccount,
                      double amount,
                      const std::string& description = "");
    std::vector<Transaction> getTransactionsByDate(
           const std::chrono::system_clock::time_point& startDate,
           const std::chrono::system_clock::time_point& endDate) const; //Вернуть транзакции за период.

    std::vector<Transaction> getTransactionsByClient(int clientId) const; //получаем всех транзакций клиента

    std::vector<Transaction> getTransactionsByAccount(const std::string& accountNumber) const; //получаем все транзакции по конретному счету

    void updateTotalAssets(); //пересчетат активы

    double getTotalClientDeposits() const; //суммируем деньги на депозитах

    double getTotalCreditDebt() const; //суммируем кредиты

    std::map<std::string, double> getBalanceSummary() const; //словарь: депозит - сумма, кредит - сумма и тд. Сводка по счетам

    std::vector<std::shared_ptr<Account>> getActiveAccounts() const; //получить только активные счета

    std::vector<std::shared_ptr<Account>> getAccountsByType(const std::string& type) const; // возвращаем счета определенного типа.

    void applyInterestToAllAccounts(); //начислить проценты по всем счетам. Логика начисления мб разной
};