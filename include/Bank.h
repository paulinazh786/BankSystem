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
    std::string name;
    std::vector<std::shared_ptr<Client>> clients;
    std::vector<std::shared_ptr<Employee>> employees;
    std::vector<std::shared_ptr<Account>> accounts;
    std::vector<Transaction> transactions;
    double totalAssets;

    static int transactionIdCounter;

public:
    explicit Bank(const std::string& bankName);

    std::string getName() const { return name; }

    const std::vector<std::shared_ptr<Client>>& getClients() const { return clients; }

    const std::vector<std::shared_ptr<Employee>>& getEmployees() const { return employees; }

    const std::vector<std::shared_ptr<Account>>& getAccounts() const { return accounts; }

    const std::vector<Transaction>& getTransactions() const { return transactions; }

    double getTotalAssets() const { return totalAssets; }

    void addClient(std::shared_ptr<Client> client);

    bool removeClient(int clientId);

    std::optional<std::shared_ptr<Client>> findClientById(int id) const;

    std::optional<std::shared_ptr<Client>> findClientByPassport(const std::string& passport) const;

    std::vector<std::shared_ptr<Client>> findClientsByName(const std::string& name) const;

    void hireEmployee(std::shared_ptr<Employee> employee);

    bool fireEmployee(int employeeId);

    std::optional<std::shared_ptr<Employee>> findEmployeeById(int id) const;

    void openAccount(std::shared_ptr<Account> account);

    bool closeAccount(const std::string& accountNumber);

    std::optional<std::shared_ptr<Account>> findAccountByNumber(const std::string& accountNumber) const;

    std::vector<std::shared_ptr<Account>> getAccountsByClient(int clientId) const;

    bool performTransaction(const Transaction& transaction);

    bool transferMoney(const std::string& fromAccount,
                      const std::string& toAccount,
                      double amount,
                      const std::string& description = "");
