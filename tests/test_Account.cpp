#include <gtest/gtest.h>
#include "../include/Account.h"
#include "../include/Client.h"
#include "../include/DebitAccount.h"
#include <chrono>
#include <memory>

class AccountTest : public ::testing::Test {
protected:
    void SetUp() override {
        birthDate = std::chrono::system_clock::now() - std::chrono::hours(365 * 25);
        client = std::make_shared<Client>(
            1, "Иван", "Иванов", "Иванович",
            birthDate, "1234 567890", "+7-900-123-45-67",
            "ivan@example.com", 750
        );
        account = std::make_shared<DebitAccount>(
            "ACC001", client, 0.05, 1000.0
        );
    }

    std::chrono::system_clock::time_point birthDate;
    std::shared_ptr<Client> client;
    std::shared_ptr<Account> account;
};

TEST_F(AccountTest, ConstructorTest) {
    EXPECT_EQ(account->getAccountNumber(), "ACC001");
    EXPECT_DOUBLE_EQ(account->getBalance(), 1000.0);
    EXPECT_TRUE(account->getIsActive());
    EXPECT_EQ(account->getOwner()->getId(), client->getId());
}

TEST_F(AccountTest, DepositTest) {
    EXPECT_TRUE(account->deposit(500.0, "Пополнение"));
    EXPECT_DOUBLE_EQ(account->getBalance(), 1500.0);
    EXPECT_EQ(account->getTransactionHistory().size(), 1);
}

TEST_F(AccountTest, WithdrawTest) {
    EXPECT_TRUE(account->withdraw(300.0, "Снятие"));
    EXPECT_DOUBLE_EQ(account->getBalance(), 700.0);
    
    EXPECT_FALSE(account->withdraw(1000.0, "Попытка снять больше баланса"));
    EXPECT_DOUBLE_EQ(account->getBalance(), 700.0);
}

TEST_F(AccountTest, TransferTest) {
    auto account2 = std::make_shared<DebitAccount>(
        "ACC002", client, 0.05, 500.0
    );
    
    EXPECT_TRUE(account->transfer(account2, 200.0, "Перевод"));
    EXPECT_DOUBLE_EQ(account->getBalance(), 800.0);
    EXPECT_DOUBLE_EQ(account2->getBalance(), 700.0);
}

TEST_F(AccountTest, ActivateDeactivateTest) {
    account->deactivate();
    EXPECT_FALSE(account->getIsActive());
    
    EXPECT_THROW(account->deposit(100.0), std::runtime_error);
    
    account->activate();
    EXPECT_TRUE(account->getIsActive());
    EXPECT_TRUE(account->deposit(100.0));
}

TEST_F(AccountTest, GetTransactionsByDateTest) {
    account->deposit(100.0, "Первое пополнение");
    
    auto startDate = std::chrono::system_clock::now() - std::chrono::hours(1);
    auto endDate = std::chrono::system_clock::now() + std::chrono::hours(1);
    
    auto transactions = account->getTransactionsByDate(startDate, endDate);
    EXPECT_GE(transactions.size(), 1);
}
