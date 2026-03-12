#include "../include/Account.h"
#include "../include/Client.h"
#include "../include/Transaction.h"
#include <algorithm>
#include <stdexcept>
#include <cmath>

//конструктор, инициализируем поля
Account::Account(const std::string& accountNumber,
                 std::shared_ptr<Client> owner,
                 double initialBalance)
    : accountNumber(accountNumber),
      balance(initialBalance),
      owner(owner),
      openDate(std::chrono::system_clock::now()),
      isActive(true) {
    if (accountNumber.empty()) {
        throw std::invalid_argument("Account number cannot be empty"); //Если номер пустой, кидаем искл
    }
    if (!owner) {
        throw std::invalid_argument("Owner cannot be null");
    }
    if (initialBalance < 0) {
        throw std::invalid_argument("Initial balance cannot be negative");
    }
}

// lock() делает так: если клиент есть -возвращает shared_ptr<Client>, если клиент уже удалён возвращает nullptr
//owner хранится как weak_ptr<Client>, его нельзя использовать напрямую как обычный объект
//Счёт не владеет клиентом, но может попробовать получить доступ к нему.
//Так избегается циклическая ссылка: Client владеет Account, Account не владеет Client, только знает о нём

std::shared_ptr<Client> Account::getOwner() const {
    return owner.lock();
}

//Берёт всю историю транзакций и оставляет только те, чья дата попадает в указанный интервал.
//проходим по элементам, провевяем условие, если истина - копируем в вектор резалт
std::vector<Transaction> Account::getTransactionsByDate(
    const std::chrono::system_clock::time_point& startDate,
    const std::chrono::system_clock::time_point& endDate) const {
    std::vector<Transaction> result;
    std::copy_if(transactions.begin(), transactions.end(),
                 std::back_inserter(result),
                //лямбда - берем транзакциб t, смотрим дату, возвращаем тру если она между старт и энд
                 [&startDate, &endDate](const Transaction& t) {
                     return t.getDate() >= startDate && t.getDate() <= endDate;
                 });
    return result;
}

//Пытается положить деньги на счёт. проверяем активен ли счет. Неактивен - исключение
//если сумма невалидна, возвращаем фолс
//пополняем баланс
bool Account::deposit(double amount, const std::string& description) {
    if (!isActive) {
        throw std::runtime_error("Cannot deposit to inactive account");
    }
    if (!validateOperation(amount)) {
        return false;
    }

    balance += amount;
//делаем запись об операции
    Transaction transaction(
        accountNumber + "_" + std::to_string(transactions.size()), //айди
        Transaction::Type::Deposit, //тип транзакции пополнение
        amount, //сумма
        shared_from_this(), //текущий объект Account получает shared_ptr на самого себя
        description
    );
    addTransaction(transaction); //Транзакция записывается в журнал
    return true;
}


//снимаем деньги со счета. Если счет неактивен - исклбчение. Если сумма плохая - фолс.

bool Account::withdraw(double amount, const std::string& description) {
    if (!isActive) {
        throw std::runtime_error("Cannot withdraw from inactive account");
    }
    if (!validateOperation(amount)) {
        return false;
    }
    if (balance < amount) { //если счет обычный, то не может уйти в минус
        return false;
    }

    balance -= amount;
    Transaction transaction(
        accountNumber + "_" + std::to_string(transactions.size()),
        Transaction::Type::Withdrawal,
        amount,
        shared_from_this(),
        description
    );
    addTransaction(transaction);
    return true;
}


//Переводит деньги с текущего счёта на другой счёт.
bool Account::transfer(std::shared_ptr<Account> to, double amount, const std::string& description) {
    if (!isActive) {
        throw std::runtime_error("Cannot transfer from inactive account");
    }
    if (!to) {
        throw std::invalid_argument("Destination account cannot be null");
    }
    if (!to->getIsActive()) {
        throw std::runtime_error("Cannot transfer to inactive account");
    }
    if (!validateOperation(amount)) {
        return false;
    }
    if (balance < amount) {
        return false;
    }

    balance -= amount;
    Transaction fromTransaction(
        accountNumber + "_" + std::to_string(transactions.size()),
        Transaction::Type::Transfer,
        amount,
        shared_from_this(),
        to,
        description
    );
    addTransaction(fromTransaction); //Добавляет транзакцию в историю текущего счёта.

    to->getBalanceRef() += amount; //Увеличивает баланс счёта-получателя.
    //Создаёт транзакцию, которая будет записана в историю счёта-получателя.
    Transaction toTransaction(
        to->accountNumber + "_" + std::to_string(to->transactions.size()),
        Transaction::Type::Transfer,
        amount,
        shared_from_this(), //текущий счёт, то есть отправитель
        to,
        description
    );
    to->addTransaction(toTransaction);

    return true;
}

//Активирует счёт
void Account::activate() {
    isActive = true;
}

void Account::deactivate() {
    isActive = false;
}
//Добавляет транзакцию в конец вектора transactions
void Account::addTransaction(const Transaction& transaction) {
    transactions.push_back(transaction);
}
//Проверяет, корректна ли сумма операции.
bool Account::validateOperation(double amount) const {
    return amount > 0 && std::isfinite(amount);
}
//прибавляет сумму к балансу. создаёт транзакцию типа Interest. записывает её в историю
bool Account::applyInterestPayment(double amount, const std::string& description) {
    if (!isActive) {
        return false;
    }
    if (!validateOperation(amount)) {
        return false;
    }

    balance += amount;
    Transaction transaction(
        accountNumber + "_interest_" + std::to_string(transactions.size()),
        Transaction::Type::Interest,
        amount,
        std::shared_ptr<Account>(nullptr), //Начисление процентов не идёт с другого счёта клиента
        shared_from_this(), //назначение = текущий счёт
        description
    );
    addTransaction(transaction);
    return true;
}