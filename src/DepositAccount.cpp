#include "../include/DepositAccount.h"
#include "../include/Client.h"
#include <stdexcept>
#include <algorithm>
#include <cmath>

DepositAccount::DepositAccount(const std::string& accountNumber,
                               std::shared_ptr<Client> owner,
                               double percent,
                               const std::chrono::system_clock::time_point& endDate,
                               bool isRefillable,
                               PaymentSchedule paymentSchedule,
                               bool isCapitalization,
                               double initialBalance)
    : Account(accountNumber, owner, initialBalance),
      percent(percent),
      isRefillable(isRefillable),
      endDate(endDate),
      paymentSchedule(paymentSchedule),
      isCapitalization(isCapitalization) {
    if (percent < 0 || percent > 1.0) {
        throw std::invalid_argument("Percent must be between 0 and 1.0");
    }
    if (endDate <= getOpenDate()) {
        throw std::invalid_argument("End date must be after open date");
    }
    if (initialBalance <= 0) {
        throw std::invalid_argument("Deposit initial balance must be positive");
    }
}

int DepositAccount::getDaysUntilMaturity() const {
    auto now = std::chrono::system_clock::now();
    if (now >= endDate) {
        return 0;
    }
    auto duration = std::chrono::duration_cast<std::chrono::hours>(endDate - now);
    return duration.count() / 24;
}

void DepositAccount::setPercent(double newPercent) {
    if (newPercent < 0 || newPercent > 1.0) {
        throw std::invalid_argument("Percent must be between 0 and 1.0");
    }
    percent = newPercent;
}

double DepositAccount::calculateInterestForPeriod(int days) const {
    if (days <= 0 || !getIsActive()) {
        return 0.0;
    }
    return getBalance() * percent * days / 365.0;
}

bool DepositAccount::canWithdraw() const {
    return isExpired();
}

bool DepositAccount::canDeposit() const {
    return isRefillable && !isExpired() && getIsActive();
}

double DepositAccount::getMaturityAmount() const {
    if (isExpired()) {
        return getBalance();
    }

    auto duration = std::chrono::duration_cast<std::chrono::hours>(endDate - getOpenDate());
    int totalDays = duration.count() / 24;

    if (isCapitalization && paymentSchedule == PaymentSchedule::Monthly) {
        int months = totalDays / 30;
        double monthlyRate = percent / 12.0;
        return getBalance() * std::pow(1.0 + monthlyRate, months);
    } else {
        return getBalance() * (1.0 + percent * totalDays / 365.0);
    }
}

void DepositAccount::applyMonthlyInterest() {
    if (!isCapitalization || !getIsActive() || isExpired()) {
        return;
    }

    if (paymentSchedule == PaymentSchedule::Monthly) {
        double monthlyInterest = calculateInterestForPeriod(30);
        applyInterestPayment(monthlyInterest, "Monthly interest capitalization");
    }
}

bool DepositAccount::isExpired() const {
    return std::chrono::system_clock::now() >= endDate;
}

bool DepositAccount::deposit(double amount, const std::string& description) {
    if (!canDeposit()) {
        throw std::runtime_error("Cannot deposit to this deposit account");
    }
    return Account::deposit(amount, description);
}

bool DepositAccount::withdraw(double amount, const std::string& description) {
    if (!canWithdraw()) {
        throw std::runtime_error("Cannot withdraw from deposit account before maturity");
    }
    return Account::withdraw(amount, description);
}

double DepositAccount::calculateInterest() const {
    if (!getIsActive() || isExpired()) {
        return 0.0;
    }

    int daysRemaining = getDaysUntilMaturity();
    if (daysRemaining <= 0) {
        return 0.0;
    }

    return calculateInterestForPeriod(daysRemaining);
}

std::string DepositAccount::getAccountType() const {
    return "Deposit";
}

bool DepositAccount::applyInterestPayment(double amount, const std::string& description) {
    if (!getIsActive() || isExpired()) {
        return false;
    }
    if (amount <= 0) {
        return false;
    }


    return Account::applyInterestPayment(amount, description);
}
