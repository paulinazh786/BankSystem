#include <gtest/gtest.h>
#include "../include/DepositAccount.h"
#include "../include/Client.h"
#include <chrono>
#include <memory>

class DepositAccountTest : public ::testing::Test {
protected:
    void SetUp() override {
        birthDate = std::chrono::system_clock::now() - std::chrono::hours(365 * 25);
        client = std::make_shared<Client>(
            1, "Иван", "Иванов", "Иванович",
            birthDate, "1234 567890", "+7-900-123-45-67",
            "ivan@example.com", 750
        );
        endDate = std::chrono::system_clock::now() + std::chrono::hours(365 * 24);
        account = std::make_shared<DepositAccount>(
            "DEPOSIT001", client, 0.08, endDate,
            false, DepositAccount::PaymentSchedule::AtEnd, true, 50000.0
        );
    }

    std::chrono::system_clock::time_point birthDate;
    std::chrono::system_clock::time_point endDate;
    std::shared_ptr<Client> client;
    std::shared_ptr<DepositAccount> account;
};

TEST_F(DepositAccountTest, ConstructorTest) {
    EXPECT_EQ(account->getAccountType(), "Deposit");
    EXPECT_DOUBLE_EQ(account->getPercent(), 0.08);
    EXPECT_FALSE(account->getIsRefillable());
    EXPECT_TRUE(account->getIsCapitalization());
}
TEST_F(DepositAccountTest, CanWithdrawTest) {
    EXPECT_FALSE(account->canWithdraw());  // Срок не истек

    EXPECT_THROW(account->withdraw(1000.0), std::runtime_error);
}

TEST_F(DepositAccountTest, CanDepositTest) {
    EXPECT_FALSE(account->canDeposit());  // Непополняемый депозит

    EXPECT_THROW(account->deposit(1000.0), std::runtime_error);
}

TEST_F(DepositAccountTest, GetDaysUntilMaturityTest) {
    int days = account->getDaysUntilMaturity();
    EXPECT_GT(days, 0);
}

TEST_F(DepositAccountTest, GetMaturityAmountTest) {
    double maturityAmount = account->getMaturityAmount();
    EXPECT_GT(maturityAmount, account->getBalance());
}

TEST_F(DepositAccountTest, CalculateInterestTest) {
    double interest = account->calculateInterest();
    EXPECT_GE(interest, 0.0);

