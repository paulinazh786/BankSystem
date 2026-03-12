#include "../include/Transaction.h"
#include "../include/Account.h"
#include <sstream>
#include <stdexcept>

Transaction::Transaction(const std::string &id, Transaction::Type type, double amount, std::shared_ptr <Account> from,
                         std::shared_ptr <Account> to, const std::string &description) : id(id), type(type), amount(amount),
                         from(from),to(to), date(std::chrono::system_clock::now()), description(description){
    if (amount <= 0) {throw std::invalid_argument("Transaction amount must be positive")}
    if (id.empty()) {throw std::invalid_argument("Transaction id cannot be empty")}
}








Transaction::Transaction(const std::string &id, Transaction::Type type, double amount,
                         std::shared_ptr <Account> account, const std::string &description) : id(id), type(type),
                         amount(amount), date(std::chrono::system_clock::now()), description(description) {
    if (amount <= 0) {throw std::invalid_argument("Transaction amount must be positive")}
    if (id.empty()) {throw std::invalid_argument("Transaction id cannot be empty")}
    if (!account) {throw std::invalid_argument("Account cannot be null")}

    if (type == Type::Deposit) {to = account;}
    else if (type == Type::Withdrawal) {from  = account}
    else {throw std::invalid_argument("Single account constructor is only for deposit or withdrawal";)}
}
std::shared_ptr<Account> Transaction::getFromAccount() const {return from.lock();}
std::shared_ptr<Account> Transaction::getToAccount() const {return to.lock();}

std::string Transaction::getTypeString() const {
    switch (type) {
        case Type::Deposit: return "Deposit";
        case Type::Withdrawal: return "Withdrawal";
        case Type::Transfer: return "Transfer";
        case Type::Interest: return "Interest";
        case Type::Fee: return "Fee";
        default: return "Unknown";

    }
}

std::string Transaction::getSummary() const {
    std::ostringstream oss;
    oss << getTypeString() << " of " << amount << " on " << id;
    if (!description.empty()) {
        oss << " (" << description << ")";
    }
    return oss.str();
}

bool Transaction::operator<(const Transaction &other) const {
    return date < other.date;
}
