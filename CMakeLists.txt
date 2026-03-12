cmake_minimum_required(VERSION 3.20)
project(BankSystem VERSION 1.0.0 LANGUAGES CXX)

# Установка стандарта C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Опции компилятора
if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Директории
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Включаемые директории
include_directories(${CMAKE_SOURCE_DIR}/include)

# Исходные файлы
set(SOURCES
        src/Person.cpp
        src/Client.cpp
        src/Employee.cpp
        src/Account.cpp
        src/Transaction.cpp
        src/DebitAccount.cpp
        src/CreditAccount.cpp
        src/DepositAccount.cpp
        src/Bank.cpp
)

# Заголовочные файлы
set(HEADERS
        include/Person.h
        include/Client.h
        include/Employee.h
        include/Account.h
        include/Transaction.h
        include/DebitAccount.h
        include/CreditAccount.h
        include/DepositAccount.h
        include/Bank.h
)

# Основная библиотека
add_library(BankSystemLib STATIC ${SOURCES} ${HEADERS})

# Исполняемый файл
add_executable(BankSystem src/main.cpp)
target_link_libraries(BankSystem BankSystemLib)

# Google Test
include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
)
FetchContent_MakeAvailable(googletest)

# Тесты
enable_testing()
add_subdirectory(tests)

# Установка
install(TARGETS BankSystem BankSystemLib
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
)

install(FILES ${HEADERS} DESTINATION include)
