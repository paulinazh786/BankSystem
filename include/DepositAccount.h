#pragma once

#include "Account.h"
#include <string>
#include <memory>
#include <chrono>

class DepositAccount : public Account {
public:

    enum class PaymentSchedule {
        AtEnd,
        Monthly
    };

private:
    double percent;
    bool isRefillable;
    std::chrono::system_clock::time_point endDate;
    PaymentSchedule paymentSchedule;
    bool isCapitalization;

    DepositAccount(const std::string& accountNumber,
                      std::shared_ptr<Client> owner,
                      double percent,
                      const std::chrono::system_clock::time_point& endDate,
                      bool isRefillable = false,
                      PaymentSchedule paymentSchedule = PaymentSchedule::AtEnd,
                      bool isCapitalization = false,
                      double initialBalance = 0.0);


    double getPercent() const { return percent; }

    bool getIsRefillable() const { return isRefillable; }

    std::chrono::system_clock::time_point getEndDate() const { return endDate; }

    int getDaysUntilMaturity() const;

    PaymentSchedule getPaymentSchedule() const { return paymentSchedule; }

    bool getIsCapitalization() const { return isCapitalization; }

