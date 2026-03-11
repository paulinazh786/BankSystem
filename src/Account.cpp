#include "../include/Account.h"
#include "../include/Client.h"
#include "../include/Transaction.h"
#include <algorithm>
#include <stdexcept>
#include <cmath>

Account::Account(const std::string& accountNumber,
                 std::shared_ptr<Client> owner,
                 double initialBalance)
    : accountNumber(accountNumber),
      balance(initialBalance),
      owner(owner),
      openDate(std::chrono::system_clock::now()),
      isActive(true) {
    if (accountNumber.empty()) {
        throw std::invalid_argument("Account number cannot be empty");
    }
    if (!owner) {
        throw std::invalid_argument("Owner cannot be null");
    }
    if (initialBalance < 0) {
        throw std::invalid_argument("Initial balance cannot be negative");
    }
}

std::shared_ptr<Client> Account::getOwner() const {
    return owner.lock();
}

std::vector<Transaction> Account::getTransactionsByDate(
    const std::chrono::system_clock::time_point& startDate,
    const std::chrono::system_clock::time_point& endDate) const {
    std::vector<Transaction> result;
    std::copy_if(transactions.begin(), transactions.end(),
                 std::back_inserter(result),
                 [&startDate, &endDate](const Transaction& t) {
                     return t.getDate() >= startDate && t.getDate() <= endDate;
                 });
    return result;
}

bool Account::deposit(double amount, const std::string& description) {
    if (!isActive) {
        throw std::runtime_error("Cannot deposit to inactive account");
    }
    if (!validateOperation(amount)) {
        return false;
    }

    balance += amount;
    Transaction transaction(
        accountNumber + "_" + std::to_string(transactions.size()),
        Transaction::Type::Deposit,
        amount,
        shared_from_this(),
        description
    );
    addTransaction(transaction);
    return true;
}

bool Account::withdraw(double amount, const std::string& description) {
    if (!isActive) {
        throw std::runtime_error("Cannot withdraw from inactive account");
    }
    if (!validateOperation(amount)) {
        return false;
    }
    if (balance < amount) {
        return false;
    }

    balance -= amount;
    Transaction transaction(
        accountNumber + "_" + std::to_string(transactions.size()),
        Transaction::Type::Withdrawal,
        amount,
        shared_from_this(),
        description
    );
    addTransaction(transaction);
    return true;
}

bool Account::transfer(std::shared_ptr<Account> to, double amount, const std::string& description) {
    if (!isActive) {
        throw std::runtime_error("Cannot transfer from inactive account");
    }
    if (!to) {
        throw std::invalid_argument("Destination account cannot be null");
    }
    if (!to->getIsActive()) {
        throw std::runtime_error("Cannot transfer to inactive account");
    }
    if (!validateOperation(amount)) {
        return false;
    }
    if (balance < amount) {
        return false;
    }

    balance -= amount;
    Transaction fromTransaction(
        accountNumber + "_" + std::to_string(transactions.size()),
        Transaction::Type::Transfer,
        amount,
        shared_from_this(),
        to,
        description
    );
    addTransaction(fromTransaction);

    to->getBalanceRef() += amount;
    Transaction toTransaction(
        to->accountNumber + "_" + std::to_string(to->transactions.size()),
        Transaction::Type::Transfer,
        amount,
        shared_from_this(),
        to,
        description
    );
    to->addTransaction(toTransaction);

    return true;
}
