#include <gtest/gtest.h>
#include "../include/Transaction.h"
#include "../include/DebitAccount.h"
#include "../include/Client.h"
#include <chrono>
#include <memory>
#include <thread>

class TransactionTest : public ::testing::Test {
protected:
    void SetUp() override {
        birthDate = std::chrono::system_clock::now() - std::chrono::hours(365 * 25);
        client = std::make_shared<Client>(
                1, "Иван", "Иванов", "Иванович",
                birthDate, "1234 567890", "+7-900-123-45-67",
                "ivan@example.com", 750
        );
        account1 = std::make_shared<DebitAccount>(
                "ACC001", client, 0.05, 1000.0
        );
        account2 = std::make_shared<DebitAccount>(
                "ACC002", client, 0.05, 500.0
        );
    }

    std::chrono::system_clock::time_point birthDate;
    std::shared_ptr<Client> client;
    std::shared_ptr<DebitAccount> account1;
    std::shared_ptr<DebitAccount> account2;
};

TEST_F(TransactionTest, DepositTransactionTest) {
Transaction transaction(
        "TXN001", Transaction::Type::Deposit, 500.0, account1, "Пополнение"
);

EXPECT_EQ(transaction.getId(), "TXN001");
EXPECT_EQ(transaction.getType(), Transaction::Type::Deposit);
EXPECT_DOUBLE_EQ(transaction.getAmount(), 500.0);
EXPECT_TRUE(transaction.isDeposit());
EXPECT_FALSE(transaction.isWithdrawal());
EXPECT_FALSE(transaction.isTransfer());
}

TEST_F(TransactionTest, WithdrawalTransactionTest) {
Transaction transaction(
        "TXN002", Transaction::Type::Withdrawal, 200.0, account1, "Снятие"
);

EXPECT_EQ(transaction.getType(), Transaction::Type::Withdrawal);
EXPECT_TRUE(transaction.isWithdrawal());
}

TEST_F(TransactionTest, TransferTransactionTest) {
Transaction transaction(
        "TXN003", Transaction::Type::Transfer, 300.0, account1, account2, "Перевод"
);

EXPECT_EQ(transaction.getType(), Transaction::Type::Transfer);
EXPECT_TRUE(transaction.isTransfer());
EXPECT_EQ(transaction.getFromAccount()->getAccountNumber(), "ACC001");
EXPECT_EQ(transaction.getToAccount()->getAccountNumber(), "ACC002");
}

TEST_F(TransactionTest, GetTypeStringTest) {
Transaction deposit("TXN001", Transaction::Type::Deposit, 100.0, account1);
EXPECT_EQ(deposit.getTypeString(), "Deposit");

Transaction withdrawal("TXN002", Transaction::Type::Withdrawal, 100.0, account1);
EXPECT_EQ(withdrawal.getTypeString(), "Withdrawal");

Transaction transfer("TXN003", Transaction::Type::Transfer, 100.0, account1, account2);
EXPECT_EQ(transfer.getTypeString(), "Transfer");
}

TEST_F(TransactionTest, ComparisonOperatorTest) {
Transaction t1("TXN001", Transaction::Type::Deposit, 100.0, account1);
std::this_thread::sleep_for(std::chrono::milliseconds(10));
Transaction t2("TXN002", Transaction::Type::Deposit, 200.0, account1);

EXPECT_TRUE(t1 < t2);
EXPECT_FALSE(t2 < t1);
}

TEST_F(TransactionTest, InvalidAmountTest) {
EXPECT_THROW(
        Transaction("TXN001", Transaction::Type::Deposit, -100.0, account1),
        std::invalid_argument
);

EXPECT_THROW(
        Transaction("TXN001", Transaction::Type::Deposit, 0.0, account1),
        std::invalid_argument
);
}