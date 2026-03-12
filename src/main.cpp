#include <iostream>
#include <memory>
#include <chrono>
#include <limits>
#include "Bank.h"
#include "Client.h"
#include "Employee.h"
#include "DebitAccount.h"
#include "CreditAccount.h"
#include "DepositAccount.h"

#ifdef _WIN32
#define NOMINMAX  // Отключаем макросы min/max из windows.h
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#endif

using namespace std::chrono_literals;

//Установка кодировки консоли для корректного отображения русского текста

void setupConsoleEncoding() {
#ifdef _WIN32
    // Устанавливаем кодировку UTF-8 для консоли Windows
    SetConsoleOutputCP(65001);  // UTF-8 для вывода
    SetConsoleCP(65001);        // UTF-8 для ввода
#endif
}

//Пример использования банковской системы
//Демонстрирует создание банка, клиентов, сотрудников и счетов,
//выполнение операций и использование современных возможностей C++23.

int main() {
    // Устанавливаем кодировку консоли для корректного отображения русского текста
    setupConsoleEncoding();

    try {
        // Создание банка
        auto bank = std::make_unique<Bank>("Тестовый Банк");
        std::cout << "=== Банк создан: " << bank->getName() << " ===\n\n";

        // Создание клиентов
        auto birthDate1 = std::chrono::system_clock::now() - std::chrono::hours(365 * 25);
        auto client1 = std::make_shared<Client>(
            1, "Иван", "Иванов", "Иванович",
            birthDate1, "1234 567890", "+7-900-123-45-67",
            "ivan@example.com", 750
        );
        bank->addClient(client1);
        std::cout << "Клиент добавлен: " << client1->getFullName()
                  << " (ID: " << client1->getId() << ")\n";

        auto birthDate2 = std::chrono::system_clock::now() - std::chrono::hours(365 * 30);
        auto client2 = std::make_shared<Client>(
            2, "Мария", "Петрова", "Сергеевна",
            birthDate2, "9876 543210", "+7-900-987-65-43",
            "maria@example.com", 800
        );
        bank->addClient(client2);
        std::cout << "Клиент добавлен: " << client2->getFullName()
                  << " (ID: " << client2->getId() << ")\n\n";

        // Создание сотрудника
        auto empBirthDate = std::chrono::system_clock::now() - std::chrono::hours(365 * 28);
        auto hireDate = std::chrono::system_clock::now() - std::chrono::hours(365 * 2);
        auto employee = std::make_shared<Employee>(
            1, "Алексей", "Сидоров", "Владимирович",
            empBirthDate, "5555 111111", "Менеджер",
            50000.0, hireDate, 10000.0, 850
        );
        bank->hireEmployee(employee);
        std::cout << "Сотрудник нанят: " << employee->getFullName()
                  << " (" << employee->getPosition() << ")\n";
        std::cout << "Стаж: " << employee->getExperience() << " лет\n";
        std::cout << "Годовой доход: " << employee->calculateYearlyIncome() << " руб.\n\n";

        // Создание дебетового счета
        auto debitAccount = std::make_shared<DebitAccount>(
            "DEBIT001", client1, 0.05, 10000.0
        );
        bank->openAccount(debitAccount);
        client1->addAccount(debitAccount);
        std::cout << "Дебетовый счет открыт: " << debitAccount->getAccountNumber()
                  << " (Баланс: " << debitAccount->getBalance() << " руб.)\n";
        std::cout << "Процент на остаток: " << (debitAccount->getPercent() * 100) << "%\n";
        std::cout << "Месячная прибыль: " << debitAccount->calculateMonthlyProfit() << " руб.\n\n";

        // Создание кредитного счета
        auto creditAccount = std::make_shared<CreditAccount>(
            "CREDIT001", client1, 50000.0, 0.15, 30
        );
        bank->openAccount(creditAccount);
        client1->addAccount(creditAccount);
        std::cout << "Кредитный счет открыт: " << creditAccount->getAccountNumber()
                  << " (Лимит: " << creditAccount->getCreditLimit() << " руб.)\n";
        std::cout << "Доступный кредит: " << creditAccount->getAvailableCredit() << " руб.\n\n";

        // Создание депозитного счета
        auto depositEndDate = std::chrono::system_clock::now() + std::chrono::hours(365 * 24);
        auto depositAccount = std::make_shared<DepositAccount>(
            "DEPOSIT001", client2, 0.08, depositEndDate,
            false, DepositAccount::PaymentSchedule::AtEnd, true, 50000.0
        );
        bank->openAccount(depositAccount);
        client2->addAccount(depositAccount);
        std::cout << "Депозитный счет открыт: " << depositAccount->getAccountNumber()
                  << " (Баланс: " << depositAccount->getBalance() << " руб.)\n";
        std::cout << "Процент: " << (depositAccount->getPercent() * 100) << "%\n";
        std::cout << "Дней до окончания: " << depositAccount->getDaysUntilMaturity() << "\n";
        std::cout << "Итоговая сумма: " << depositAccount->getMaturityAmount() << " руб.\n\n";

        // Операции со счетами
        std::cout << "=== Операции ===\n";

        // Пополнение дебетового счета
        debitAccount->deposit(5000.0, "Пополнение счета");
        std::cout << "Пополнение дебетового счета: +5000 руб.\n";
        std::cout << "Новый баланс: " << debitAccount->getBalance() << " руб.\n\n";

        // Снятие с кредитного счета
        creditAccount->withdraw(10000.0, "Снятие средств");
        std::cout << "Снятие с кредитного счета: -10000 руб.\n";
        std::cout << "Баланс: " << creditAccount->getBalance() << " руб.\n";
        std::cout << "Задолженность: " << creditAccount->getDebt() << " руб.\n";
        std::cout << "Доступный кредит: " << creditAccount->getAvailableCredit() << " руб.\n\n";

        // Перевод между счетами
        bank->transferMoney("DEBIT001", "CREDIT001", 2000.0, "Погашение кредита");
        std::cout << "Перевод 2000 руб. с DEBIT001 на CREDIT001\n";
        std::cout << "Баланс дебетового: " << debitAccount->getBalance() << " руб.\n";
        std::cout << "Баланс кредитного: " << creditAccount->getBalance() << " руб.\n\n";

        // Начисление процентов
        std::cout << "=== Начисление процентов ===\n";
        bank->applyInterestToAllAccounts();
        std::cout << "Проценты начислены на все счета\n\n";

        // Финансовая статистика
        std::cout << "=== Финансовая статистика банка ===\n";
        bank->updateTotalAssets();
        std::cout << "Общие активы: " << bank->getTotalAssets() << " руб.\n";
        std::cout << "Общая сумма депозитов: " << bank->getTotalClientDeposits() << " руб.\n";
        std::cout << "Общая задолженность по кредитам: " << bank->getTotalCreditDebt() << " руб.\n";

        auto balanceSummary = bank->getBalanceSummary();
        std::cout << "\nБалансы по типам счетов:\n";
        for (const auto& [type, balance] : balanceSummary) {
            std::cout << "  " << type << ": " << balance << " руб.\n";
        }

        // История транзакций клиента
        std::cout << "\n=== История транзакций клиента " << client1->getFullName() << " ===\n";
        auto clientTransactions = bank->getTransactionsByClient(client1->getId());
        for (const auto& transaction : clientTransactions) {
            std::cout << transaction.getSummary() << "\n";
        }

        std::cout << "\n=== Программа завершена успешно ===\n";

    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        std::cout << "\nНажмите Enter для выхода...";
        std::cin.clear();
        // Используем скобки для обхода конфликта с макросом max из windows.h
        std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
        std::cin.get();
        return 1;
    }

    std::cout << "\nНажмите Enter для выхода...";
    std::cin.clear();
    // Используем скобки для обхода конфликта с макросом max из windows.h
    std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
    std::cin.get();
    return 0;
}
