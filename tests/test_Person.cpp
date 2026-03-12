#include <gtest/gtest.h>
#include "../include/Person.h"
#include <chrono>

class PersonTest : public ::testing::Test {
protected:
    void SetUp() override {
        birthDate = std::chrono::system_clock::now() - std::chrono::hours(365 * 25);
    }

    std::chrono::system_clock::time_point birthDate;
};


class TestPerson : public Person {
public:
    TestPerson(const std::string& firstName,
               const std::string& lastName,
               const std::string& patronymic,
               const std::chrono::system_clock::time_point& birthDate,
               const std::string& passportNumber,
               int creditRating = 500)
            : Person(firstName, lastName, patronymic, birthDate, passportNumber, creditRating) {}

    std::string getRole() const override {
        return "Test";
    }
};

TEST_F(PersonTest, ConstructorTest) {
TestPerson person("Иван", "Иванов", "Иванович", birthDate, "1234 567890", 750);

EXPECT_EQ(person.getFirstName(), "Иван");
EXPECT_EQ(person.getLastName(), "Иванов");
EXPECT_EQ(person.getPatronymic(), "Иванович");
EXPECT_EQ(person.getPassportNumber(), "1234 567890");
EXPECT_EQ(person.getCreditRating(), 750);
}

TEST_F(PersonTest, GetFullNameTest) {
TestPerson person("Иван", "Иванов", "Иванович", birthDate, "1234 567890");
std::string fullName = person.getFullName();
EXPECT_TRUE(fullName.find("Иванов") != std::string::npos);
EXPECT_TRUE(fullName.find("Иван") != std::string::npos);
}

TEST_F(PersonTest, SetCreditRatingTest) {
TestPerson person("Иван", "Иванов", "Иванович", birthDate, "1234 567890", 500);

person.setCreditRating(800);
EXPECT_EQ(person.getCreditRating(), 800);

EXPECT_THROW(person.setCreditRating(-1), std::invalid_argument);
EXPECT_THROW(person.setCreditRating(1001), std::invalid_argument);
}

TEST_F(PersonTest, UpdatePassportTest) {
TestPerson person("Иван", "Иванов", "Иванович", birthDate, "1234 567890");

person.updatePassport("9876 543210");
EXPECT_EQ(person.getPassportNumber(), "9876 543210");

EXPECT_THROW(person.updatePassport(""), std::invalid_argument);
}
