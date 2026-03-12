#include <gtest/gtest.h>
#include "../include/Employee.h"
#include <chrono>
#include <memory>

class EmployeeTest : public ::testing::Test {
protected:
    void SetUp() override {
        birthDate = std::chrono::system_clock::now() - std::chrono::hours(365 * 28);
        hireDate = std::chrono::system_clock::now() - std::chrono::hours(365 * 2);
        employee = std::make_shared<Employee>(
            1, "Алексей", "Сидоров", "Владимирович",
            birthDate, "5555 111111", "Менеджер",
            50000.0, hireDate, 10000.0, 850
        );
    }
    std::chrono::system_clock::time_point birthDate;
    std::chrono::system_clock::time_point hireDate;
    std::shared_ptr<Employee> employee;
};

TEST_F(EmployeeTest, ConstructorTest) {
    EXPECT_EQ(employee->getId(), 1);
    EXPECT_EQ(employee->getPosition(), "Менеджер");
    EXPECT_DOUBLE_EQ(employee->getSalary(), 50000);
    EXPECT_DOUBLE_EQ(employee->getBonus(), 10000);
    EXPECT_EQ(employee->getRole(), "Employee");
}

TEST_F(EmployeeTest, GetTotalCompensationTest) {
    EXPECT_DOUBLE_EQ(employee->getTotalCompensation(), 60000);
}

TEST_F(EmployeeTest, CalculateYearlyIncomeTest) {
    double yearlyIncome = employee->calculateYearlyIncome();
    EXPECT_DOUBLE_EQ(yearlyIncome, 50000.0 * 12.0 + 10000.0);
}

TEST_F(EmployeeTest, GiveRaiseTest) {
    employee->giveRaise(10.0);
    EXPECT_DOUBLE_EQ(employee->getSalary(), 55000.0);

    EXPECT_THROW(employee->giveRaise(-5.0), std::invalid_argument);
}

TEST_F(EmployeeTest, SetSalaryAndBonusTest) {
    employee->setSalary(60000.0);
    EXPECT_DOUBLE_EQ(employee->getSalary(), 60000.0);

    employee->setBonus(15000.0);
    EXPECT_DOUBLE_EQ(employee->getBonus(), 15000.0);

    EXPECT_THROW(employee->setSalary(-1000.0), std::invalid_argument);
    EXPECT_THROW(employee->setBonus(-1000.0), std::invalid_argument);
}

TEST_F(EmployeeTest, SetPositionTest) {
    employee->setPositiom("Старший менеджер")ж
    EXPECT_EQ(employee->getPosition(), "Старший менеджер");

    EXPECT_THROW(employee->setPosition(""), std::invalid_argument);
}
