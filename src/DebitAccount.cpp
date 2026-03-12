#include "../include/DebitAccount.h"
#include "../include/Client.h"
#include <stdexcept>
#include <cmath>

DebitAccount::DebitAccount(const std::string& accountNumber,
                           std::shared_ptr<Client> owner,
                           double percent,
                           double initialBalance)
    : Account(accountNumber, owner, initialBalance),
      percent(percent) {
    if (percent < 0 || percent > 1.0) {
        throw std::invalid_argument("Percent must be between 0 and 1.0");
    }
}

void DebitAccount::setPercent(double newPercent) {
    if (newPercent < 0 || newPercent > 1.0) {
        throw std::invalid_argument("Percent must be between 0 and 1.0");
    }
    percent = newPercent;
}

double DebitAccount::calculateMonthlyProfit() const {
    if (!getIsActive() || getBalance() <= 0) {
        return 0.0;
    }
    return getBalance() * percent / 12.0;
}

double DebitAccount::calculateInterest() const {
    return calculateMonthlyProfit();
}

std::string DebitAccount::getAccountType() const {
    return "Debit";
}
