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

void DebitAccount::setPercent(double newPercent) {}

double DebitAccount::calculateMonthlyProfit() const {}

double DebitAccount::calculateInterest() const {}

std::string DebitAccount::getAccountType() const {}