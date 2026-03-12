#include <gtest/gtest.h>
#include "../include/DebitAccount.h"
#include "../include/Client.h"
#include <chrono>
#include <memory>

class DebitAccountTest : public ::testing::Test {
protected:
    void SetUp() override {
        birthDate = std::chrono::system_clock::now() - std::chrono::hours(365 * 25);
        client = std::make_shared<Client>(
            1, "Иван", "Иванов", "Иванович",
            birthDate, "1234 567890", "+7-900-123-45-67",
            "ivan@example.com", 750
        );
        account = std::make_shared<DebitAccount>(
            "DEBIT001", client, 0.05, 10000.0
        );
    }
    std::chrono::system_clock::time_point birthDate;
    std::shared_ptr<Client> client;
    std::shared_ptr<DebitAccount> account;
};

TEST_F(DebitAccountTest, ConstructorTest) {
    EXPECT_EQ(account->getAccountType(), "Debit");
    EXPECT_DOUBLE_EQ(account->getPercent(), 0.05);
    EXPECT_DOUBLE_EQ(account->getBalance(), 10000.0);
}

TEST_F(DebitAccountTest, CalculateMonthlyProfitTest) {
    double profit = account->calculateMonthlyProfit();
    EXPECT_GT(profit, 0.0);
    EXPECT_NEAR(profit, 41.67, 0.1);
}

TEST_F(DebitAccountTest, CalculateInterestTest) {
    double interest = account->calculateInterest();
    EXPECT_GT(interest, 0.0);
}

TEST_F(DebitAccountTest, SetPercentTest) {
    account->setPercent(0.06);
    EXPECT_DOUBLE_EQ(account->getPercent(), 0.06);
    EXPECT_THROW(account->setPercent(-0.1), std::invalid_argument);
    EXPECT_THROW(account->setPercent(1.5), std::invalid_argument);
}
