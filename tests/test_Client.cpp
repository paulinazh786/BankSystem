#include <gtest/gtest.h>
#include "../include/Client.h"
#include "../include/DebitAccount.h"
#include <chrono>
#include <memory>

class ClientTest : public ::testing::Test {
protected:
    void SetUp() override {
        birthDate = std::chrono::system_clock::now() - std::chrono::hours(365 * 25);
        client = std::make_shared<Client>(
                1, "Иван", "Иванов", "Иванович",
                birthDate, "1234 567890", "+7-900-123-45-67",
                "ivan@example.com", 750
        );
    }

    std::chrono::system_clock::time_point birthDate;
    std::shared_ptr<Client> client;
};

TEST_F(ClientTest, ConstructorTest) {
EXPECT_EQ(client->getId(), 1);
EXPECT_EQ(client->getPhone(), "+7-900-123-45-67");
EXPECT_EQ(client->getEmail(), "ivan@example.com");
EXPECT_EQ(client->getRole(), "Client");
}

TEST_F(ClientTest, AddAccountTest) {
auto account = std::make_shared<DebitAccount>(
        "ACC001", client, 0.05, 1000.0
);

client->addAccount(account);
EXPECT_TRUE(client->hasAccount("ACC001"));
EXPECT_EQ(client->getAccounts().size(), 1);
}

TEST_F(ClientTest, GetTotalBalanceTest) {
auto account1 = std::make_shared<DebitAccount>(
        "ACC001", client, 0.05, 1000.0
);
auto account2 = std::make_shared<DebitAccount>(
        "ACC002", client, 0.05, 2000.0
);

client->addAccount(account1);
client->addAccount(account2);

EXPECT_DOUBLE_EQ(client->getTotalBalance(), 3000.0);
}

TEST_F(ClientTest, RemoveAccountTest) {
auto account = std::make_shared<DebitAccount>(
        "ACC001", client, 0.05, 1000.0
);

client->addAccount(account);
EXPECT_TRUE(client->hasAccount("ACC001"));

EXPECT_TRUE(client->removeAccount("ACC001"));
EXPECT_FALSE(client->hasAccount("ACC001"));

EXPECT_FALSE(client->removeAccount("NONEXISTENT"));
}

TEST_F(ClientTest, SetPhoneAndEmailTest) {
client->setPhone("+7-900-999-99-99");
EXPECT_EQ(client->getPhone(), "+7-900-999-99-99");

client->setEmail("newemail@example.com");
EXPECT_EQ(client->getEmail(), "newemail@example.com");

EXPECT_THROW(client->setPhone(""), std::invalid_argument);
EXPECT_THROW(client->setEmail(""), std::invalid_argument);
}
