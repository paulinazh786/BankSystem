#include "../include/Client.h"
#include "../include/Account.h"
#include <algorithm>
#include <stdexcept>

Client::Client(int id, const std::string &firstName, const std::string &lastName, const std::string &patronymic,
               const std::chrono::system_clock::time_point &birthDate, const std::string &passportNumber, const std::string& phone,
               const std::string& email, int creditRating) : Person(firstName, lastName, patronymic, birthDate, passportNumber, creditRating),
               id(id), phone(phone), email(email) {if (id < 0){ throw std::invalid_argument("Client id must be non-negative");}}


std::shared_ptr<Account> Client::getAccount(const std::string& accountNumber) const {
    auto it = std::find_if(accounts.begin(), accounts.end(),
                           [&accountNumber](const std::shared_ptr<Account>& acc) {
        return acc->getAccountNumber() == accountNumber;});
    return (it != accounts.end()) ? *it : nullptr;
}
double Client::getTotalBalance() const {
    double total = 0.0;
    for (const auto& account : accounts) {total += account->getBalance();}
    return total;
}

void Client::addAccount(std::shared_ptr<Account> account) {
    if (!account) {
        throw std::invalid_argument("Account cannot be null");
    }
    accounts.push_back(account);
}

bool Client::removeAccount(const std::string &accountNumber) {
    auto it = std::remove_if(accounts.begin(), accounts.end(), [&accountNumber](const std::shared_ptr<Account>& acc) {
        return acc->getAccountNumber() == accountNumber;
    });
    if (it != accounts.end()) {
        accounts.erase(it, accounts.end());
        return true;
    }
    return false;
}

bool Client::hasAccount(const std::string& accountNumber) const {return getAccount(accountNumber) != nullptr;}

void Client::setPhone(const std::string &newPhone) {
    if (newPhone.empty()){
        throw std::invalid_argument("Phone number cannot be empty");
    }
    phone = newPhone;
}

void Client::setEmail(const std::string &newEmail) {
    if (newEmail.empty()){
        throw std::invalid_argument("Email number cannot be empty");
    }
    email = newEmail;
}

std::string Client::getRole() const {
    return "Client";
}