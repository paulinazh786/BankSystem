#pragma once

#include <string>
#include <chrono>
#include <memory>

class Person {
protected:
    std::string lastName;
    std::string firstName;
    std::string patronymic;
    std::chrono::system_clock::time_point birthDate;
    int creditRating;
    std::string passportNumber;
public:
    Person(const std::string& firstName, const std::string& lastName, const std::string& patronymic,
           const std::chrono::system_clock::time_point& birthDate, const std::string& passportNumber, int creditRating = 500);

    virtual ~Person() = default;

    std::string getLastName() const {return lastName}
    std::string getFirstName() const {return firstName}
    std::string getPatronymic() const {return patronymic}
    std::string getFullName() const;

    std::chrono::systen_clock::time_point getBirthdate() const {return birthDate}
    int getAge() const;

    int getCreditRating() const {return creditRating;}
    std::string getPassportNumber() const {return passportNumber;}

    void setCreditRating(int rating);

    void updatePassport(const std::string& newPassport);
    virtual std::string getRole() const = 0;


};