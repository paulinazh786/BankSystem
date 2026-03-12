#pragma once

#include "Person.h"
#include <vector>
#include <memory>
#include <string>

class Account;


class Client : public Person {
private:
    int id;
    std::string phone;
    std::string email;
    std::vector<std::shared_ptr<Account>> accounts;
public:
    Client (int id, const std::string& firstName, const std::string& lastName, const std::string& patronymic,
            std::chrono::system_clock::time_point& birthDate, const std::string& passportNumber, std::string phone,
            std::string email, int creditRating = 500);

    int getId() const {return id;}
    std::string getPhone() const {return phone;}
    std::string getEmail() const {return email;}
    const std::vector<std::shared_ptr<Account>>& getAccounts() const {return account;}
    double getTotalBalance() const;

    void addAccount(std::shared_ptr<Account> account);
    bool removeAccount(const std::string& accountNumber);
    bool hasAccount(const std::string& accountNumber) const;

    void setPhone(const std::string& newPhone);
    void setEmail(const std::string& newEmail);

    std::string getRole() const override;

};