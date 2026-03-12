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

double CreditAccount::getAvailableCredit() const {}

double CreditAccount::getDebt() const {}

void CreditAccount::setCreditLimit(double newLimit) {}

void CreditAccount::setPercent(double newPercent) {}

bool CreditAccount::isInGracePeriod() const {}

double CreditAccount::calculatePenalty() const {}

bool CreditAccount::withdraw(double amount, const std::string& description) {}

double CreditAccount::calculateInterest() const {}

std::string CreditAccount::getAccountType() const {}
