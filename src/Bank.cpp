#include "../include/Bank.h"
#include <algorithm>
#include <stdexcept>
#include <sstream>

int Bank::transactionIdCounter = 0;

Bank::Bank(const std::string& bankName)
    : name(bankName), totalAssets(0.0) {
    if (bankName.empty()) {
        throw std::invalid_argument("Bank name cannot be empty");
    }
}

void Bank::addClient(std::shared_ptr<Client> client) {
    if (!client) {
        throw std::invalid_argument("Client cannot be null");
    }

    // Проверяем, нет ли уже клиента с таким ID или паспортом
    auto existingById = findClientById(client->getId());
    if (existingById.has_value()) {
        throw std::runtime_error("Client with this ID already exists");
    }

    auto existingByPassport = findClientByPassport(client->getPassportNumber());
    if (existingByPassport.has_value()) {
        throw std::runtime_error("Client with this passport already exists");
    }

    clients.push_back(client);
}

bool Bank::removeClient(int clientId) {
    auto it = std::remove_if(clients.begin(), clients.end(),
        [clientId](const std::shared_ptr<Client>& c) {
            return c->getId() == clientId;
        });

    if (it != clients.end()) {
        clients.erase(it, clients.end());
        updateTotalAssets();
        return true;
    }
    return false;
}

std::optional<std::shared_ptr<Client>> Bank::findClientById(int id) const {
    auto it = std::find_if(clients.begin(), clients.end(),
        [id](const std::shared_ptr<Client>& c) {
            return c->getId() == id;
        });

    return (it != clients.end()) ? std::make_optional(*it) : std::nullopt;
}

std::optional<std::shared_ptr<Client>> Bank::findClientByPassport(const std::string& passport) const {
    auto it = std::find_if(clients.begin(), clients.end(),
        [&passport](const std::shared_ptr<Client>& c) {
            return c->getPassportNumber() == passport;
        });

    return (it != clients.end()) ? std::make_optional(*it) : std::nullopt;
}

std::vector<std::shared_ptr<Client>> Bank::findClientsByName(const std::string& name) const {
    std::vector<std::shared_ptr<Client>> result;
    std::copy_if(clients.begin(), clients.end(),
                 std::back_inserter(result),
                 [&name](const std::shared_ptr<Client>& c) {
                     return c->getFirstName() == name ||
                            c->getLastName() == name ||
                            c->getFullName().find(name) != std::string::npos;
                 });
    return result;
}

void Bank::hireEmployee(std::shared_ptr<Employee> employee) {
    if (!employee) {
        throw std::invalid_argument("Employee cannot be null");
    }

    auto existing = findEmployeeById(employee->getId());
    if (existing.has_value()) {
        throw std::runtime_error("Employee with this ID already exists");
    }

    employees.push_back(employee);
}

bool Bank::fireEmployee(int employeeId) {
    auto it = std::remove_if(employees.begin(), employees.end(),
        [employeeId](const std::shared_ptr<Employee>& e) {
            return e->getId() == employeeId;
        });

    if (it != employees.end()) {
        employees.erase(it, employees.end());
        return true;
    }
    return false;
}

std::optional<std::shared_ptr<Employee>> Bank::findEmployeeById(int id) const {
    auto it = std::find_if(employees.begin(), employees.end(),
        [id](const std::shared_ptr<Employee>& e) {
            return e->getId() == id;
        });

    return (it != employees.end()) ? std::make_optional(*it) : std::nullopt;
}

void Bank::openAccount(std::shared_ptr<Account> account) {
    if (!account) {
        throw std::invalid_argument("Account cannot be null");
    }

    auto existing = findAccountByNumber(account->getAccountNumber());
    if (existing.has_value()) {
        throw std::runtime_error("Account with this number already exists");
    }

    accounts.push_back(account);
    updateTotalAssets();
}

bool Bank::closeAccount(const std::string& accountNumber) {
    auto accountOpt = findAccountByNumber(accountNumber);
    if (!accountOpt.has_value()) {
        return false;
    }

    auto account = accountOpt.value();
    account->deactivate();
    updateTotalAssets();
    return true;
}

std::optional<std::shared_ptr<Account>> Bank::findAccountByNumber(const std::string& accountNumber) const {
    auto it = std::find_if(accounts.begin(), accounts.end(),
        [&accountNumber](const std::shared_ptr<Account>& a) {
            return a->getAccountNumber() == accountNumber;
        });

    return (it != accounts.end()) ? std::make_optional(*it) : std::nullopt;
}

std::vector<std::shared_ptr<Account>> Bank::getAccountsByClient(int clientId) const {
    std::vector<std::shared_ptr<Account>> result;
    std::copy_if(accounts.begin(), accounts.end(),
                 std::back_inserter(result),
                 [clientId](const std::shared_ptr<Account>& a) {
                     auto owner = a->getOwner();
                     return owner && owner->getId() == clientId;
                 });
    return result;
}

bool Bank::performTransaction(const Transaction& transaction) {
    transactions.push_back(transaction);
    updateTotalAssets();
    return true;
}

bool Bank::transferMoney(const std::string& fromAccount,
                         const std::string& toAccount,
                         double amount,
                         const std::string& description) {
    auto fromOpt = findAccountByNumber(fromAccount);
    auto toOpt = findAccountByNumber(toAccount);

    if (!fromOpt.has_value() || !toOpt.has_value()) {
        return false;
    }

    auto from = fromOpt.value();
    auto to = toOpt.value();

    if (from->transfer(to, amount, description)) {
        std::ostringstream oss;
        oss << "TXN_" << (++transactionIdCounter);
        Transaction transaction(
            oss.str(),
            Transaction::Type::Transfer,
            amount,
            from,
            to,
            description
        );
        performTransaction(transaction);
        return true;
    }

    return false;
}

std::vector<Transaction> Bank::getTransactionsByDate(
    const std::chrono::system_clock::time_point& startDate,
    const std::chrono::system_clock::time_point& endDate) const {
    std::vector<Transaction> result;
    std::copy_if(transactions.begin(), transactions.end(),
                 std::back_inserter(result),
                 [&startDate, &endDate](const Transaction& t) {
                     return t.getDate() >= startDate && t.getDate() <= endDate;
                 });
    return result;
}

std::vector<Transaction> Bank::getTransactionsByClient(int clientId) const {
    std::vector<Transaction> result;
    for (const auto& transaction : transactions) {
        auto from = transaction.getFromAccount();
        auto to = transaction.getToAccount();

        if ((from && from->getOwner() && from->getOwner()->getId() == clientId) ||
            (to && to->getOwner() && to->getOwner()->getId() == clientId)) {
            result.push_back(transaction);
        }
    }
    return result;
}

std::vector<Transaction> Bank::getTransactionsByAccount(const std::string& accountNumber) const {
    std::vector<Transaction> result;
    std::copy_if(transactions.begin(), transactions.end(),
                 std::back_inserter(result),
                 [&accountNumber](const Transaction& t) {
                     auto from = t.getFromAccount();
                     auto to = t.getToAccount();
                     return (from && from->getAccountNumber() == accountNumber) ||
                            (to && to->getAccountNumber() == accountNumber);
                 });
    return result;
}

void Bank::updateTotalAssets() {
    totalAssets = 0.0;
    for (const auto& account : accounts) {
        if (account->getIsActive()) {
            totalAssets += account->getBalance();
        }
    }
}

double Bank::getTotalClientDeposits() const {
    double total = 0.0;
    for (const auto& account : accounts) {
        if (account->getAccountType() == "Deposit" && account->getIsActive()) {
            total += account->getBalance();
        }
    }
    return total;
}

double Bank::getTotalCreditDebt() const {
    double total = 0.0;
    for (const auto& account : accounts) {
        if (account->getAccountType() == "Credit" && account->getIsActive()) {
            if (account->getBalance() < 0) {
                total += std::abs(account->getBalance());
            }
        }
    }
    return total;
}


std::map<std::string, double> Bank::getBalanceSummary() const {
    std::map<std::string, double> summary;
    for (const auto& account : accounts) {
        if (account->getIsActive()) {
            std::string type = account->getAccountType();
            summary[type] += account->getBalance();
        }
    }
    return summary;
}

std::vector<std::shared_ptr<Account>> Bank::getActiveAccounts() const {
    std::vector<std::shared_ptr<Account>> result;
    std::copy_if(accounts.begin(), accounts.end(),
                 std::back_inserter(result),
                 [](const std::shared_ptr<Account>& a) {
                     return a->getIsActive();
                 });
    return result;
}

std::vector<std::shared_ptr<Account>> Bank::getAccountsByType(const std::string& type) const {
    std::vector<std::shared_ptr<Account>> result;
    std::copy_if(accounts.begin(), accounts.end(),
                 std::back_inserter(result),
                 [&type](const std::shared_ptr<Account>& a) {
                     return a->getAccountType() == type;
                 });
    return result;
}

void Bank::applyInterestToAllAccounts() {
    for (const auto& account : accounts) {
        if (account->getIsActive()) {
            double interest = account->calculateInterest();
            if (interest > 0) {
                std::ostringstream oss;
                oss << "TXN_INTEREST_" << (++transactionIdCounter);
                Transaction interestTransaction(
                    oss.str(),
                    Transaction::Type::Interest,
                    interest,
                    nullptr,
                    account,
                    "Interest payment"
                );
                // Используем applyInterestPayment для обхода ограничений депозитов
                if (account->applyInterestPayment(interest, "Interest payment")) {
                    performTransaction(interestTransaction);
                }
            }
        }
    }
}
