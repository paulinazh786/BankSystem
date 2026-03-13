#include "../include/CreditAccount.h"
#include "../include/Client.h"
#include <stdexcept>
#include <algorithm>
#include <cmath>

CreditAccount::CreditAccount(const std::string& accountNumber,
                             std::shared_ptr<Client> owner,
                             double creditLimit,
                             double percent,
                             int woPercentPeriod,
                             double initialBalance)
    : Account(accountNumber, owner, initialBalance),
      creditLimit(creditLimit),
      percent(percent),
      woPercentPeriod(woPercentPeriod),
      lastUsageDate(std::chrono::system_clock::now()) {
    if (creditLimit <= 0) {
        throw std::invalid_argument("Credit limit must be positive");
    }
    if (percent < 0 || percent > 1.0) {
        throw std::invalid_argument("Percent must be between 0 and 1.0");
    }
    if (woPercentPeriod < 0) {
        throw std::invalid_argument("Grace period cannot be negative");
    }
}

double CreditAccount::getAvailableCredit() const {
    if (getBalance() >= 0) {
        return creditLimit;
    }
    double usedCredit = std::abs(getBalance());
    return std::max(0.0, creditLimit - usedCredit);
}

double CreditAccount::getDebt() const {
    return (getBalance() < 0) ? std::abs(getBalance()) : 0.0;
}

void CreditAccount::setCreditLimit(double newLimit) {
    if (newLimit <= 0) {
        throw std::invalid_argument("Credit limit must be positive");
    }
    creditLimit = newLimit;
}

void CreditAccount::setPercent(double newPercent) {
    if (newPercent < 0 || newPercent > 1.0) {
        throw std::invalid_argument("Percent must be between 0 and 1.0");
    }
    percent = newPercent;
}

bool CreditAccount::isInGracePeriod() const {
    if (woPercentPeriod == 0) {
        return false;
    }
    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::hours>(now - lastUsageDate);
    int days = duration.count() / 24;
    return days <= woPercentPeriod;
}

double CreditAccount::calculatePenalty() const {
    if (getBalance() >= 0 || isInGracePeriod()) {
        return 0.0;
    }
    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::hours>(now - lastUsageDate);
    int daysOverdue = (duration.count() / 24) - woPercentPeriod;
    if (daysOverdue <= 0) {
        return 0.0;
    }
    return getDebt() * 0.01 * daysOverdue;
}

bool CreditAccount::withdraw(double amount, const std::string& description) {
    if (!getIsActive()) {
        throw std::runtime_error("Cannot withdraw from inactive account");
    }
    if (amount <= 0 || !std::isfinite(amount)) {
        return false;
    }

    double newBalance = getBalance() - amount;
    if (newBalance < -creditLimit) {
        return false;
    }

    balance = newBalance;

    Transaction transaction(
        accountNumber + "_" + std::to_string(transactions.size()),
        Transaction::Type::Withdrawal,
        amount,
        shared_from_this(),
        description
    );
    addTransaction(transaction);


    if (newBalance < 0) {
        lastUsageDate = std::chrono::system_clock::now();
    }

    return true;
}

double CreditAccount::calculateInterest() const {
    if (getBalance() >= 0 || isInGracePeriod()) {
        return 0.0;
    }
    double debt = getDebt();
    return debt * percent / 365.0;
}

std::string CreditAccount::getAccountType() const {
    return "Credit";
}
