#pragma once
#include "Account.h"
#include <string>
#include <memory>
#include <chrono>

class CreditAccount : public Account {
private:
    double creditLimit;
    double percent;
    int woPercentPeriod;
    std::chrono::system_clock::time_point lastUsageDate;
public:
    CreditAccount(const std::string& accountNumber,
                  std::shared_ptr<Client> owner,
                  double creditLimit,
                  double percent,
                  int woPercentPeriod = 0,
                  double initialBalance = 0.0);

    double getCreditLimit() const { return creditLimit; }

    double getPercent() const { return percent; }

    int getWoPercentPeriod() const { return woPercentPeriod; }

    double getAvailableCredit() const;

    double getDebt() const;

    void setCreditLimit(double newLimit);

    void setPercent(double newPercent);

    bool isInGracePeriod() const;

    double calculatePenalty() const;

    bool withdraw(double amount, const std::string& description = "") override;

    double calculateInterest() const override;

    std::string getAccountType() const override;
};
