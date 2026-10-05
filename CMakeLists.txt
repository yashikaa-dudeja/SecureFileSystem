cmake_minimum_required(VERSION 3.15)
project(SecureCapabilityFileSystem VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

set(COMMON_SOURCES
    src/ProcessManager.cpp
    src/VirtualFileSystem.cpp
    src/CapabilityManager.cpp
    src/AuditLogger.cpp
    src/AccessController.cpp
)

# Main CLI Application
add_executable(secure_fs
    ${COMMON_SOURCES}
    src/main.cpp
)

# Automated Test Suite Runner
add_executable(test_runner
    ${COMMON_SOURCES}
    tests/TestRunner.cpp
)

enable_testing()
add_test(NAME CoreCapabilitySecurityTests COMMAND test_runner)
