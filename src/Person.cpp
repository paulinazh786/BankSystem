#include "../include/Person.h"
#include <sstream>
#include <ctime>

Person::Person(const std::string &firstName, const std::string &lastName, const std::string &patronymic,
               const std::chrono::system_clock::time_point &birthDate, const std::string &passportNumber, int creditRating)
               : firstName(firstName), lastName(lastName), patronymic(patronymic), birthDate(birthDate),
               passportNumber(passportNumber), creditRating(creditRating) {
    if (firstName.empty()) {
        throw std::invalid_argument("First name cannot be empty");
    }
    if (lastName.empty()) {
        throw std::invalid_argument("Last name cannot be empty");
    }
    if (birthDate > std::chrono::system_clock::now()) {
        throw std::invalid_argument("Birthdate cannot be in the future");
    }
    setCreditRating(creditRating);
    updatePassport(passportNumber);
}

std::string Person::getFullName() const {
    std::ostringstream oss;
    oss << lastName << " " << firstName << " " << patronymic;
    return oss.str();
}

int Person::getAge() const {
    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::hours>(now - birthDate);
    int years = duration.count() / (365*24);
    return years;
}

void Person::setCreditRating(int rating) {
    if (rating < 0 || rating > 1000) {
        throw std::invalid_argument("Credit rating must be between 0 and 1000");
    }
    creditRating = rating;
}

void Person::updatePassport(const std::string &newPassport) {
    if (newPassport.empty()) {
        throw std::invalid_argument("Passport number cannot be empty");
    }
    passportNumber = newPassport;
}