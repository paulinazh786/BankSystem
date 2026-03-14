#include "../include/Transaction.h"
#include "../include/Account.h"
#include <sstream>
#include <stdexcept>
#include <variant>

Transaction::Transaction(const std::string &id, Transaction::Type type, double amount, std::shared_ptr <Account> from,
                         std::shared_ptr <Account> to, const std::string &description) : id(id), type(type), amount(amount),
                         from(from),to(to), date(std::chrono::system_clock::now()), description(description){
    if (amount <= 0) {throw std::invalid_argument("Transaction amount must be positive");}
    if (id.empty()) {throw std::invalid_argument("Transaction id cannot be empty");}
}

Transaction::Transaction(const std::string &id, Transaction::Type type, double amount,
                         std::shared_ptr <Account> account, const std::string &description) : id(id), type(type),
                         amount(amount), date(std::chrono::system_clock::now()), description(description) {
    if (amount <= 0) {throw std::invalid_argument("Transaction amount must be positive");}
    if (id.empty()) {throw std::invalid_argument("Transaction id cannot be empty");}
    if (!account) {throw std::invalid_argument("Account cannot be null");}

    if (type == Type::Deposit) {to = account;}
    else if (type == Type::Withdrawal) {from  = account;}
    else {throw std::invalid_argument("Single account constructor is only for deposit or withdrawal");}
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
    using TransactionDetails = std::variant<
        std::monostate,
        std::string,
        std::pair<std::string, std::string>
    >;

    TransactionDetails details = std::monostate{};

    auto fromAccount = getFromAccount();
    auto toAccount = getToAccount();

    if (type == Type::Deposit && toAccount) {
        details = toAccount->getAccountNumber();
    } else if (type == Type::Withdrawal && fromAccount) {
        details = fromAccount->getAccountNumber();
    } else if (type == Type::Transfer && fromAccount && toAccount) {
        details = std::make_pair(
            fromAccount->getAccountNumber(),
            toAccount->getAccountNumber()
        );
    }

    std::ostringstream oss;
    oss << getTypeString() << " of " << amount << " on " << id;

    std::visit([&oss](const auto& value) {
        using T = std::decay_t<decltype(value)>;

        if constexpr (std::is_same_v<T, std::monostate>) {
            // no additional account info
        } else if constexpr (std::is_same_v<T, std::string>) {
            oss << " [account: " << value << "]";
        } else if constexpr (std::is_same_v<T, std::pair<std::string, std::string>>) {
            oss << " [from: " << value.first << ", to: " << value.second << "]";
        }
    }, details);

    if (!description.empty()) {
        oss << " (" << description << ")";
    }

    return oss.str();
}

bool Transaction::operator<(const Transaction &other) const {
    return date < other.date;
}
