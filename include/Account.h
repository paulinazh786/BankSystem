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
