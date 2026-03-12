#include "../include/Bank.h"
#include <algorithm>
#include <stdexcept>
#include <sstream>

int Bank::transactionIdCounter = 0; //общий счетчик трназакций чтобы создавать уникальные айди. если банков будет 2, то счетчик 1


//конструктор банка. задаем имя, активы 0.0, если имя пусто, то выбрасывается ошибка
Bank::Bank(const std::string& bankName)
    : name(bankName), totalAssets(0.0) {
    if (bankName.empty()) {
        throw std::invalid_argument("Bank name cannot be empty");
    }
}

//добавляем клиента. проверяем, что клиент вообще передан.
void Bank::addClient(std::shared_ptr<Client> client) {
    if (!client) {
        throw std::invalid_argument("Client cannot be null");
    }

    // Проверяем, нет ли уже клиента с таким ID
    auto existingById = findClientById(client->getId()); //обращение к объекту через shared_ptr
    if (existingById.has_value()) {
        throw std::runtime_error("Client with this ID already exists");
    }
      //проверяем нет ли клиентам с такми же паспортом
    auto existingByPassport = findClientByPassport(client->getPassportNumber());
    if (existingByPassport.has_value()) {
        throw std::runtime_error("Client with this passport already exists");
    }
    //добавляем
    clients.push_back(client);
}

//удаляем клиента. проходим по списку
bool Bank::removeClient(int clientId) {
    auto it = std::remove_if(clients.begin(), clients.end(), //remove переставляет все нужные элементы в начало, возвращает итератор на новый конец
        [clientId](const std::shared_ptr<Client>& c) { //список захвата айди,
            return c->getId() == clientId;
        });

    if (it != clients.end()) {
        clients.erase(it, clients.end()); //удаляет хвост вектора те элементы, которые remove_if пометил к удалению.
        updateTotalAssets();
        return true;
    }
    return false;
}
//ищет клиента с заданным айди. optional возвращает пустое значение, если ничего не найдено.
std::optional<std::shared_ptr<Client>> Bank::findClientById(int id) const {
    auto it = std::find_if(clients.begin(), clients.end(),
        [id](const std::shared_ptr<Client>& c) {
            return c->getId() == id;
        });

    return (it != clients.end()) ? std::make_optional(*it) : std::nullopt;
}
// то же самое, но поиск по номеру паспорта
std::optional<std::shared_ptr<Client>> Bank::findClientByPassport(const std::string& passport) const {
    auto it = std::find_if(clients.begin(), clients.end(),
        [&passport](const std::shared_ptr<Client>& c) {
            return c->getPassportNumber() == passport;
        });

    return (it != clients.end()) ? std::make_optional(*it) : std::nullopt;
}
// ищет всез клиентов у которые совпадает имя фамилия или подстрока встречается в полном имени
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
//добавляем сотрулника, проверяем что объект есть и уникальность по айди
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
//удаление сотрудника по айди
bool Bank::fireEmployee(int employeeId) {
    auto it = std::remove_if(employees.begin(), employees.end(),
        [employeeId](const std::shared_ptr<Employee>& e) {
            return e->getId() == employeeId; //пройди по всем сотруд и найди первого, у кого getId() равен нужному id
        });

    if (it != employees.end()) {
        employees.erase(it, employees.end());
        return true;
    }
    return false;
}
//поиск сотрудника.пройди по всем сотруд и найди первого, у кого getId() равен нужному id
std::optional<std::shared_ptr<Employee>> Bank::findEmployeeById(int id) const {
    auto it = std::find_if(employees.begin(), employees.end(),
        [id](const std::shared_ptr<Employee>& e) {
            return e->getId() == id;
        });

    return (it != employees.end()) ? std::make_optional(*it) : std::nullopt;
} // если сотрудник найден вернуть опшнл с найденным значением, если нет, вернуть nullptr

//добавляем новый счет в систему банка. Проверки - существует, айди уникаален.
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
//деактивирует счет по номеру
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

//ищет счет по номеру, номер счета уникальный ключ объекта
std::optional<std::shared_ptr<Account>> Bank::findAccountByNumber(const std::string& accountNumber) const {
    auto it = std::find_if(accounts.begin(), accounts.end(),
        [&accountNumber](const std::shared_ptr<Account>& a) {
            return a->getAccountNumber() == accountNumber;
        });

    return (it != accounts.end()) ? std::make_optional(*it) : std::nullopt;
}
//получение счетов клиента, возвращает все счета принадрежащие конкретному клиенту
//берем владельца через гет, проверяем что владелец существует, сравниваем owner->getId() с clientId
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
//добавляет транзакцию в общий журнал банка
bool Bank::performTransaction(const Transaction& transaction) {
    transactions.push_back(transaction);
    updateTotalAssets();
    return true;
}

//переводим деньги с одного счета на другой по номерам счетов
bool Bank::transferMoney(const std::string& fromAccount,
                         const std::string& toAccount,
                         double amount,
                         const std::string& description) {
    auto fromOpt = findAccountByNumber(fromAccount); //ищем исходящий счет
    auto toOpt = findAccountByNumber(toAccount); //ищем входящий счет

    if (!fromOpt.has_value() || !toOpt.has_value()) {
        return false; //если хоть один счет не найден, возвращаем фолс
    }

    auto from = fromOpt.value();
    auto to = toOpt.value();

    if (from->transfer(to, amount, description)) { //from — это указатель на объект счёта отправителя, вызываем у него метод трансфер
        std::ostringstream oss;
        oss << "TXN_" << (++transactionIdCounter); //если перевод успещен, создаем транзакцию
        Transaction transaction( //создаем объект транзакшн
            oss.str(),
            Transaction::Type::Transfer,
            amount,
            from,
            to,
            description
        );
        performTransaction(transaction); //записываем
        return true;
    }

    return false;
}
//фильтруем общий журнал банка по временному диапазону
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
//получение транзакций по айди клиента. Берем счета отправителя, получателя, если любой из них принадлежит клиенту, включаем транзакцию в результат
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
//возвращает все банковские транзакции, где участвовал счет с данным номером
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
//обновление суммарных активов банка
void Bank::updateTotalAssets() {
    totalAssets = 0.0;
    for (const auto& account : accounts) {
        if (account->getIsActive()) {
            totalAssets += account->getBalance();
        }
    }
}
//общая сумма депозитов
double Bank::getTotalClientDeposits() const {
    double total = 0.0;
    for (const auto& account : accounts) {
        if (account->getAccountType() == "Deposit" && account->getIsActive()) {
            total += account->getBalance();
        }
    }
    return total;
}
//общая сумма кредитов
double Bank::getTotalCreditDebt() const {
    double total = 0.0;
    for (const auto& account : accounts) {
        if (account->getAccountType() == "Credit" && account->getIsActive()) {
            if (account->getBalance() < 0) {
                total += std::abs(account->getBalance()); //берем модуль от отриц баланса
            }
        }
    }
    return total;
}

//сводка по балансам, собирает словарь. Депозит - сумма, кредиты - сумма и тд
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
//геттер активных счетов
std::vector<std::shared_ptr<Account>> Bank::getActiveAccounts() const {
    std::vector<std::shared_ptr<Account>> result;
    std::copy_if(accounts.begin(), accounts.end(),
                 std::back_inserter(result),
                 [](const std::shared_ptr<Account>& a) {
                     return a->getIsActive();
                 });
    return result;
}
//геттер счетов по типу, возвращает все счета указанного типа
std::vector<std::shared_ptr<Account>> Bank::getAccountsByType(const std::string& type) const {
    std::vector<std::shared_ptr<Account>> result;
    std::copy_if(accounts.begin(), accounts.end(),
                 std::back_inserter(result),
                 [&type](const std::shared_ptr<Account>& a) {
                     return a->getAccountType() == type;
                 });
    return result;
}
//начисление процентов по всем счетам
void Bank::applyInterestToAllAccounts() {
    for (const auto& account : accounts) { //проходим по всем счетам
        if (account->getIsActive()) { //берем активные
            double interest = account->calculateInterest(); //считаем! проценты, полиморфизм
            if (interest > 0) { //если проценты положит. создаем банк.транзакцию
                std::ostringstream oss;
                oss << "TXN_INTEREST_" << (++transactionIdCounter);
                Transaction interestTransaction(
                    oss.str(),
                    Transaction::Type::Interest,
                    interest,
                    nullptr, //источник пуст т.к проценты не приходят с другого счета
                    account,
                    "Interest payment"
                );
                // применяем! проценты к самому счету, реально меняет состояние счета
                if (account->applyInterestPayment(interest, "Interest payment")) {
                    performTransaction(interestTransaction); //записываем в журнал
                }
            }
        }
    }
}
