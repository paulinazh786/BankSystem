#pragma once

#include "Transaction.h"
#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <optional>

class Client;

class Account : public std::enable_shared_from_this<Account> {
protected:
    std::string accountNumber;
    double balance;
    std::weak_ptr<Client> owner;
    std::chrono::system_clock::time_point openDate;
    bool isActive;
    std::vector<Transaction> transactions;

protected:
    double& getBalanceRef() { return balance; }

    +++++++++

public:
    Account(const std::string& accountNumber,
            std::shared_ptr<Client> owner,
            double initialBalance = 0.0);

    virtual ~Account() = default;

    std::string getAccountNumber() const { return accountNumber; }

    double getBalance() const { return balance; }

    std::shared_ptr<Client> getOwner() const;

    std::chrono::system_clock::time_point getOpenDate() const { return openDate; }

    bool getIsActive() const { return isActive; }

    const std::vector<Transaction>& getTransactionHistory() const { return transactions; }

    std::vector<Transaction> getTransactionsByDate(
        const std::chrono::system_clock::time_point& startDate,
        const std::chrono::system_clock::time_point& endDate) const;
    virtual bool deposit(double amount, const std::string& description = "");

    virtual bool withdraw(double amount, const std::string& description = "");

    virtual bool transfer(std::shared_ptr<Account> to, double amount, const std::string& description = "");

    void activate();

    void deactivate();

    virtual double calculateInterest() const = 0;

    virtual std::string getAccountType() const = 0;

    virtual bool applyInterestPayment(double amount, const std::string& description = "Interest payment");

protected:
    void addTransaction(const Transaction& transaction);

    bool validateOperation(double amount) const;
};