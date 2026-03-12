#include <gtest/gtest.h>
#include "../include/Bank.h"
#include "../include/Client.h"
#include "../include/Employee.h"
#include "../include/DebitAccount.h"
#include "../include/CreditAccount.h"
#include "../include/DepositAccount.h"
#include <chrono>
#include <memory>

class BankTest : public ::testing::Test {
protected:
    void SetUp() override {
        bank = std::make_unique<Bank>("Тестовый Банк");
        
        birthDate = std::chrono::system_clock::now() - std::chrono::hours(365 * 25);
        client = std::make_shared<Client>(
            1, "Иван", "Иванов", "Иванович",
            birthDate, "1234 567890", "+7-900-123-45-67",
            "ivan@example.com", 750
        );
        
        empBirthDate = std::chrono::system_clock::now() - std::chrono::hours(365 * 28);
        hireDate = std::chrono::system_clock::now() - std::chrono::hours(365 * 2);
        employee = std::make_shared<Employee>(
            1, "Алексей", "Сидоров", "Владимирович",
            empBirthDate, "5555 111111", "Менеджер",
            50000.0, hireDate, 10000.0
        );
    }

    std::unique_ptr<Bank> bank;
    std::chrono::system_clock::time_point birthDate;
    std::chrono::system_clock::time_point empBirthDate;
    std::chrono::system_clock::time_point hireDate;
    std::shared_ptr<Client> client;
    std::shared_ptr<Employee> employee;
};

TEST_F(BankTest, ConstructorTest) {
    EXPECT_EQ(bank->getName(), "Тестовый Банк");
    EXPECT_EQ(bank->getClients().size(), 0);
    EXPECT_EQ(bank->getEmployees().size(), 0);
}

TEST_F(BankTest, AddClientTest) {
    bank->addClient(client);
    EXPECT_EQ(bank->getClients().size(), 1);
    
    auto found = bank->findClientById(1);
    EXPECT_TRUE(found.has_value());
    EXPECT_EQ(found.value()->getId(), 1);
}

TEST_F(BankTest, FindClientByPassportTest) {
    bank->addClient(client);
    
    auto found = bank->findClientByPassport("1234 567890");
    EXPECT_TRUE(found.has_value());
    EXPECT_EQ(found.value()->getPassportNumber(), "1234 567890");
    
    auto notFound = bank->findClientByPassport("9999 999999");
    EXPECT_FALSE(notFound.has_value());
}

TEST_F(BankTest, HireEmployeeTest) {
    bank->hireEmployee(employee);
    EXPECT_EQ(bank->getEmployees().size(), 1);
    
    auto found = bank->findEmployeeById(1);
    EXPECT_TRUE(found.has_value());
    EXPECT_EQ(found.value()->getId(), 1);
}

TEST_F(BankTest, OpenAccountTest) {
    bank->addClient(client);
    auto account = std::make_shared<DebitAccount>(
        "ACC001", client, 0.05, 1000.0
    );
    
    bank->openAccount(account);
    EXPECT_EQ(bank->getAccounts().size(), 1);
    
    auto found = bank->findAccountByNumber("ACC001");
    EXPECT_TRUE(found.has_value());
    EXPECT_EQ(found.value()->getAccountNumber(), "ACC001");
}

TEST_F(BankTest, TransferMoneyTest) {
    bank->addClient(client);
    auto account1 = std::make_shared<DebitAccount>(
        "ACC001", client, 0.05, 1000.0
    );
    auto account2 = std::make_shared<DebitAccount>(
        "ACC002", client, 0.05, 500.0
    );
    
    bank->openAccount(account1);
    bank->openAccount(account2);
    
    EXPECT_TRUE(bank->transferMoney("ACC001", "ACC002", 200.0, "Перевод"));
    EXPECT_DOUBLE_EQ(account1->getBalance(), 800.0);
    EXPECT_DOUBLE_EQ(account2->getBalance(), 700.0);
}

TEST_F(BankTest, GetAccountsByClientTest) {
    bank->addClient(client);
    auto account1 = std::make_shared<DebitAccount>(
        "ACC001", client, 0.05, 1000.0
    );
    auto account2 = std::make_shared<DebitAccount>(
        "ACC002", client, 0.05, 500.0
    );
    
    bank->openAccount(account1);
    bank->openAccount(account2);
    
    auto accounts = bank->getAccountsByClient(1);
    EXPECT_EQ(accounts.size(), 2);
}

TEST_F(BankTest, GetBalanceSummaryTest) {
    bank->addClient(client);
    auto debitAccount = std::make_shared<DebitAccount>(
        "DEBIT001", client, 0.05, 1000.0
    );
    auto creditAccount = std::make_shared<CreditAccount>(
        "CREDIT001", client, 5000.0, 0.15
    );
    
    bank->openAccount(debitAccount);
    bank->openAccount(creditAccount);
    
    auto summary = bank->getBalanceSummary();
    EXPECT_GT(summary.size(), 0);
}

TEST_F(BankTest, DuplicateClientTest) {
    bank->addClient(client);
    
    auto duplicate = std::make_shared<Client>(
        1, "Другой", "Человек", "Тест",
        birthDate, "9999 999999", "+7-900-999-99-99",
        "test@example.com"
    );
    
    EXPECT_THROW(bank->addClient(duplicate), std::runtime_error);
}
