cmake_minimum_required(VERSION 3.20)

# Тесты
add_executable(BankSimulationTests
        test_Person.cpp
        test_Client.cpp
        test_Employee.cpp
        test_Account.cpp
        test_Transaction.cpp
        test_DebitAccount.cpp
        test_CreditAccount.cpp
        test_DepositAccount.cpp
        test_Bank.cpp
)

target_link_libraries(BankSimulationTests
        BankSimulationLib
        GTest::gtest
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(BankSimulationTests)
