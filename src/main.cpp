#include <iostream>
#include <memory>
#include <chrono>
#include <limits>
#include <string>
#include <sstream>
#include <vector>
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
    // Устанавливаем кодировку UTF-8 для консоли Windows (ввод и вывод кириллицы)
    SetConsoleOutputCP(65001);  // UTF-8 для вывода
    SetConsoleCP(65001);        // UTF-8 для ввода
#endif
}

#ifdef _WIN32
// Чтение одной строки из консоли через Windows API (Unicode), чтобы кириллица вводилась и сохранялась корректно
static std::string readConsoleLineWin() {
    constexpr DWORD bufSize = 1024;
    wchar_t wbuf[bufSize];
    DWORD read = 0;
    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
    if (!ReadConsoleW(hIn, wbuf, bufSize - 1, &read, nullptr) || read == 0)
        return {};
    wbuf[read] = L'\0';
    std::wstring wstr(wbuf);
    // Убираем \r\n в конце
    while (!wstr.empty() && (wstr.back() == L'\r' || wstr.back() == L'\n'))
        wstr.pop_back();
    // Преобразуем UTF-16 -> UTF-8
    if (wstr.empty()) return {};
    int size = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), static_cast<int>(wstr.size()), nullptr, 0, nullptr, nullptr);
    if (size <= 0) return {};
    std::string result(size, '\0');
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), static_cast<int>(wstr.size()), &result[0], size, nullptr, nullptr);
    return result;
}
#endif

// Читает строку из консоли; при пустом вводе запрашивает снова (чтобы название банка и ФИО сохранялись)
std::string readString(const std::string& prompt) {
    std::string value;
    for (;;) {
        std::cout << prompt;
#ifdef _WIN32
        value = readConsoleLineWin();
#else
        std::getline(std::cin, value);
#endif
        // Убираем пробелы по краям
        auto start = value.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) {
            std::cout << "Пустая строка не допускается, введите снова.\n";
            continue;
        }
        auto end = value.find_last_not_of(" \t\r\n");
        value = value.substr(start, end - start + 1);
        break;
    }
    return value;
}

// Разбор ФИО из одной строки (Фамилия Имя Отчество)
void parseFullName(const std::string& fullName, std::string& lastName, std::string& firstName, std::string& patronymic) {
    lastName.clear();
    firstName.clear();
    patronymic.clear();
    std::string word;
    std::vector<std::string> parts;
    std::istringstream iss(fullName);
    while (iss >> word)
        parts.push_back(word);
    if (parts.size() >= 1) lastName = parts[0];
    if (parts.size() >= 2) firstName = parts[1];
    if (parts.size() >= 3) {
        patronymic = parts[2];
        for (size_t i = 3; i < parts.size(); ++i)
            patronymic += " " + parts[i];
    } else {
        patronymic = "Автоматически";
    }
}

double readDouble(const std::string& prompt) {
    double value;
    std::cout << prompt;
    while (!(std::cin >> value)) {
        std::cin.clear();
        std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
        std::cout << "Ошибка ввода. Введите число: ";
    }
    std::cin.ignore(); // Игнорируем символ новой строки
    return value;
}

//Демонстрация банковской системы - показ работы всех классов
//Минимальный ввод данных для быстрой демонстрации

int main() {
    // Устанавливаем кодировку консоли для корректного отображения русского текста
    setupConsoleEncoding();

    try {
        std::cout << "=== ДЕМОНСТРАЦИЯ БАНКОВСКОЙ СИСТЕМЫ ===\n\n";

        // Создание банка
        std::string bankName = readString("Введите название банка: ");
        auto bank = std::make_unique<Bank>(bankName);
        std::cout << "Банк создан: " << bank->getName() << "\n\n";

        // Создание первого клиента — ФИО одной строкой
        std::cout << "--- Создание клиента 1 ---\n";
        std::string fullName1 = readString("Введите ФИО клиента (одной строкой): ");
        std::string lastName1, firstName1, patronymic1;
        parseFullName(fullName1, lastName1, firstName1, patronymic1);

        auto birthDate1 = std::chrono::system_clock::now() - std::chrono::hours(365 * 30);
        auto client1 = std::make_shared<Client>(
            1, firstName1, lastName1, patronymic1,
            birthDate1, "0000 000000", "+7-000-000-00-00",
            "client1@example.com", 750
        );
        bank->addClient(client1);
        std::cout << "Клиент добавлен: " << client1->getFullName() << "\n\n";

        // Создание второго клиента (автоматически)
        auto birthDate2 = std::chrono::system_clock::now() - std::chrono::hours(365 * 25);
        auto client2 = std::make_shared<Client>(
            2, "Мария", "Петрова", "Сергеевна",
            birthDate2, "9876 543210", "+7-900-987-65-43",
            "maria@example.com", 800
        );
        bank->addClient(client2);
        std::cout << "Клиент добавлен: " << client2->getFullName() << "\n\n";

        // Создание сотрудника (данные без изменений)
        std::cout << "--- Найм сотрудника ---\n";
        auto empBirthDate = std::chrono::system_clock::now() - std::chrono::hours(365 * 28);
        auto hireDate = std::chrono::system_clock::now() - std::chrono::hours(365 * 2);
        auto employee = std::make_shared<Employee>(
            1, "Иван", "Сидоров", "Петрович",
            empBirthDate, "5555 111111", "Менеджер",
            50000.0, hireDate, 10000.0, 850
        );
        bank->hireEmployee(employee);
        std::cout << "Сотрудник нанят: " << employee->getFullName()
                  << " (" << employee->getPosition() << ")\n";
        std::cout << "  Стаж: " << employee->getExperience() << " лет\n";
        std::cout << "  Годовой доход: " << employee->calculateYearlyIncome() << " руб.\n\n";

        // Начальные балансы и кредитный лимит — ввод из консоли
        double initialDebitBalance = readDouble("Введите начальный баланс дебетового счёта: ");
        double creditLimit = readDouble("Введите кредитный лимит: ");
        double initialDepositBalance = readDouble("Введите начальный баланс депозитного счёта: ");

        // Создание дебетового счета
        auto debitAccount = std::make_shared<DebitAccount>(
            "DEBIT001", client1, 0.05, initialDebitBalance
        );
        bank->openAccount(debitAccount);
        client1->addAccount(debitAccount);
        std::cout << "Дебетовый счет открыт: " << debitAccount->getAccountNumber()
                  << " (Процент: 5%, баланс: " << initialDebitBalance << " руб.)\n\n";

        // Создание кредитного счета (лимит из консоли; при снятии лимит уменьшается, задолженность растёт)
        auto creditAccount = std::make_shared<CreditAccount>(
            "CREDIT001", client1, creditLimit, 0.15, 30, 0.0
        );
        bank->openAccount(creditAccount);
        client1->addAccount(creditAccount);
        std::cout << "Кредитный счет открыт: " << creditAccount->getAccountNumber()
                  << " (Лимит: " << creditLimit << " руб., доступно: " << creditAccount->getAvailableCredit() << " руб.)\n\n";

        // Создание депозитного счета
        auto depositEndDate = std::chrono::system_clock::now() + std::chrono::hours(365 * 24);
        auto depositAccount = std::make_shared<DepositAccount>(
            "DEPOSIT001", client2, 0.08, depositEndDate,
            false, DepositAccount::PaymentSchedule::AtEnd, true, initialDepositBalance
        );
        bank->openAccount(depositAccount);
        client2->addAccount(depositAccount);
        std::cout << "Депозитный счет открыт: " << depositAccount->getAccountNumber()
                  << " (Процент: 8%)\n\n";

        // Транзакции (ввод из консоли: снятие с кредитного и сумма перевода)
        std::cout << "=== ТРАНЗАКЦИИ ===\n";
        std::cout << "Текущий баланс дебетового счёта: " << debitAccount->getBalance() << " руб.\n\n";

        // Снятие с кредитного счета
        double creditWithdraw = readDouble("Введите сумму для снятия с кредитного счета: ");
        try {
            creditAccount->withdraw(creditWithdraw, "Снятие средств");
            std::cout << "Снятие с кредитного счета: -" << creditWithdraw << " руб.\n";
            std::cout << "  Баланс: " << creditAccount->getBalance() << " руб.\n";
            std::cout << "  Кредитный лимит: " << creditAccount->getCreditLimit() << " руб.\n";
            std::cout << "  Доступно по лимиту: " << creditAccount->getAvailableCredit() << " руб.\n";
            std::cout << "  Задолженность: " << creditAccount->getDebt() << " руб.\n\n";
        } catch (const std::exception& e) {
            std::cout << "✗ Ошибка: " << e.what() << "\n\n";
        }

        // Перевод между счетами
        double transferAmount = readDouble("Введите сумму перевода с дебетового на кредитный счет: ");
        try {
            bank->transferMoney("DEBIT001", "CREDIT001", transferAmount, "Погашение кредита");
            std::cout << "Перевод: " << transferAmount << " руб. с DEBIT001 на CREDIT001\n";
            std::cout << "  Баланс дебетового: " << debitAccount->getBalance() << " руб.\n";
            std::cout << "  Баланс кредитного: " << creditAccount->getBalance() << " руб.\n";
            std::cout << "  Кредитный лимит: " << creditAccount->getCreditLimit() << " руб.\n";
            std::cout << "  Задолженность по кредиту: " << creditAccount->getDebt() << " руб.\n\n";
        } catch (const std::exception& e) {
            std::cout << "✗ Ошибка перевода: " << e.what() << "\n\n";
        }

        // Начисление процентов
        std::cout << "=== Начисление процентов ===\n";
        bank->applyInterestToAllAccounts();
        std::cout << "Проценты начислены на все счета\n\n";

        // Финансовая статистика банка
        std::cout << "=== ФИНАНСОВАЯ АНАЛИТИКА ===\n";
        bank->updateTotalAssets();
        std::cout << "Общие активы: " << bank->getTotalAssets() << " руб.\n";
        std::cout << "Общая сумма депозитов: " << bank->getTotalClientDeposits() << " руб.\n";
        std::cout << "Общая задолженность по кредитам: " << bank->getTotalCreditDebt() << " руб.\n";

        auto balanceSummary = bank->getBalanceSummary();
        std::cout << "\nБалансы по типам счетов:\n";
        for (const auto& [type, balance] : balanceSummary) {
            std::cout << "  " << type << ": " << balance << " руб.\n";
        }

        std::cout << "\nБалансы клиентов:\n";
        std::cout << "  " << client1->getFullName() << ": " << client1->getTotalBalance() << " руб.\n";
        std::cout << "  " << client2->getFullName() << ": " << client2->getTotalBalance() << " руб.\n";

        // История транзакций клиента
        std::cout << "\n=== История транзакций клиента " << client1->getFullName() << " ===\n";
        auto clientTransactions = bank->getTransactionsByClient(client1->getId());
        for (const auto& transaction : clientTransactions) {
            std::cout << "  " << transaction.getSummary() << "\n";
        }

        std::cout << "\n=== Демонстрация завершена ===\n";

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