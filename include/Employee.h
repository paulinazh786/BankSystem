#pragma once

#include "Person.h"
#include <string>
#include <chrono>

class Employee : public Person {
private:
    int id;
    std::string position;
    double salary;
    std::chrono::system_clock::time_point hireDate;
    double bonus;

public:
    Employee(int id,
             const std::string& firstName,
             const std::string& lastName,
             const std::string& patronymic,
             const std::chrono::system_clock::time_point& birthDate,
             const std::string& passportNumber,
             const std::string& position,
             double salary,
             const std::chrono::system_clock::time_point& hireDate,
             double bonus = 0.0,
             int creditRating = 500);

    int getId() const { return id; }

    std::string getPosition() const { return position; }

    double getSalary() const { return salary; }

    std::chrono::system_clock::time_point getHireDate() const { return hireDate; }

    int getExperience() const;

    double getBonus() const { return bonus; }

    double getTotalCompensation() const { return salary + bonus; }


    void setPostion(const std::string newPosition);

    void setSalary(double newSalary);

    void setBonus(double newBonus);

    void giveRaise(double percent);

    double calculateYearlyIncome() const;

    std::string getRole() const override;
};