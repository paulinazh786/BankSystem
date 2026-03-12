#include "../include/Employee.h"
#include <stdexcept>

Employee::Employee(int id,
const std::string& firstName,
const std::string& lastName,
const std::string& patronymic,
const std::chrono::system_clock::time_point& birthDate,
const std::string& passportNumber,
const std::string& position,
double salary,
const std::chrono::system_clock::time_point& hireDate,
double bonus,
int creditRating)
: Person(firstName, lastName, patronymic, birthDate, passportNumber, creditRating),
id(id),
position(position),
salary(salary),
hireDate(hireDate),
bonus(bonus) {
    if (id < 0) {
        throw std::invalid_argument("Employee ID must be non-negative");
    }
    if (salary < 0) {
        throw std::invalid_argument("Salary cannot be negative");
    }
    if (bonus < 0) {
        throw std::invalid_argument("Bonus cannot be negative");
    }
}

int Employee::getExperience() const {
    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::hours>(now - hireDate);
    int years = duration.count() / (365 * 24);
    return years;
}

void Employee::setPosition(const std::string& newPosition) {
    if (newPosition.empty()) {
        throw std::invalid_argument("Position cannot be empty");
    }
    position = newPosition;
}

void Employee::setSalary(double new Salary) {
    if (newSalary < 0) {
        throw std::invalid_argument("Salary cannot be negative");
    }
    salary = newSalary;
}

