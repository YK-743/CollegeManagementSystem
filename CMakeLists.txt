cmake_minimum_required(VERSION 3.10)
project(CollegeManagementSystem LANGUAGES CXX)

# Require C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Header search path
include_directories(${PROJECT_SOURCE_DIR}/include)

# Source files
set(SOURCES
    src/main.cpp
    src/Models.cpp
    src/Managers.cpp
    src/DataManager.cpp
    src/CollegeSystem.cpp
    src/ConsoleUI.cpp
)

# Build the executable
add_executable(CollegeManagementSystem ${SOURCES})
