#pragma once

#include <string>
#include <chrono>
#include <memory>

class Account;

class Transaction {
private:
    std::string id;
    std::weak_ptr<Account> from;
    std::weak_ptr<Account> to;
    double amount;
    std::chrono::system_clock::time_point date;
    std::string description;

public:
    enum class Type {
        Deposit,
        Withdrawal,
        Transfer,
        Interest,
        Fee,
    };
    Type type;

    Transaction(const std::string& id, Type type, double amount, std::shared_ptr<Account> from,
                std::shared_ptr<Account> to, const std::string& description = "");
    Transaction(const std::string& id, Type type, double amount, std::shared_ptr<Account> account,
                const std::string& description = "");

    std::string getId() const {return id;}
    std::shared_ptr<Account> getFromAccount() const;
    std::shared_ptr<Account> getToAccount() const;
    double getAmount() const {return amount;}
    std::chrono::system_clock::time_point getDate() const {return date;}
    Type getType() const {return type;}
    std::string getTypeString() const;
    std::string getDescription() const {return description;}

    bool isDeposit() const {return type == Type::Deposit;}
    bool isWithdrawal() const {return type == Type::Withdrawal;}
    bool isTransfer() const {return type == Type::Transfer;}

    std::string getSummary() const;

    bool operator<(const Transaction& other) const;



};