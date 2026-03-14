#pragma once
#include "Account.h"
#include <string>
#include <memory>

class DebitAccount : public Account {
private:
    static constexpr int MonthsInYear = 12;
    double percent;
public:
    DebitAccount(const std::string& accountNumber,
        std::shared_ptr<Client> owner,
        double percent,
        double initialBalance = 0.0);

    double getPercent() const { return percent; }

    void setPercent(double newPercent);

    double calculateMonthlyProfit() const;

    double calculateInterest() const override;

    std::string getAccountType() const override;
};
