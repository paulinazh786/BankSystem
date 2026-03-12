#include <gtest/gtest.h>
#include "../include/CreditAccount.h"
#include "../include/Client.h"
#include <chrono>
#include <memory>

class CreditAccountTest : public ::testing::Test {
protected:
    void SetUp() override {
        birthDate = std::chrono::system_clock::now() - std::chrono::hours(365 * 25);
        client = std::make_shared<Client>(
            1, "Иван", "Иванов", "Иванович",
            birthDate, "1234 567890", "+7-900-123-45-67",
            "ivan@example.com", 750
        );
        account = std::make_shared<CreditAccount>(
            "CREDIT001", client, 50000.0, 0.15, 30
        );
    }
    std::chrono::system_clock::time_point birthDate;
    std::shared_ptr<Client> client;
    std::shared_ptr<CreditAccount> account;
};

TEST_F(CreditAccountTest, ConstructorTest) {
    EXPECT_EQ(account->getAccountType(), "Credit");
    EXPECT_DOUBLE_EQ(account->getCreditLimit(), 50000.0);
    EXPECT_DOUBLE_EQ(account->getPercent(), 0.15);
    EXPECT_EQ(account->getWoPercentPeriod(), 30);
}

TEST_F(CreditAccountTest, GetAvailableCreditTest) {
    EXPECT_DOUBLE_EQ(account->getAvailableCredit(), 50000.0);
    account->withdraw(10000.0, "Снятие");
    EXPECT_DOUBLE_EQ(account->getAvailableCredit(), 40000.0);
}

TEST_F(CreditAccountTest, GetDebtTest) {
    EXPECT_DOUBLE_EQ(account->getDebt(), 0.0);
    account->withdraw(10000.0, "Снятие");
    EXPECT_DOUBLE_EQ(account->getDebt(), 10000.0);
}

TEST_F(CreditAccountTest, WithdrawWithLimitTest) {
    EXPECT_TRUE(account->withdraw(30000.0, "Снятие"));
    EXPECT_DOUBLE_EQ(account->getBalance(), -30000.0);
    EXPECT_FALSE(account->withdraw(30000.0, "Попытка превысить лимит"));
}

TEST_F(CreditAccountTest, CalculateInterestTest) {
    account->withdraw(10000.0, "Снятие");
    double interest = account->calculateInterest();
    EXPECT_GE(interest, 0.0);
}

TEST_F(CreditAccountTest, SetCreditLimitTest) {
    account->setCreditLimit(60000.0);
    EXPECT_DOUBLE_EQ(account->getCreditLimit(), 60000.0);
    EXPECT_THROW(account->setCreditLimit(-1000.0), std::invalid_argument);
}
