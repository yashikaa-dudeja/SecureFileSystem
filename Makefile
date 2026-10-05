CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -Iinclude

# Detect Operating System
ifeq ($(OS),Windows_NT)
    TARGET = secure_fs.exe
    TEST_TARGET = test_runner.exe
    MKDIR = if not exist obj mkdir obj
    MKDIR_BIN = if not exist bin mkdir bin
    RM = del /f /q
    RMDIR = rmdir /s /q
    BIN_DIR = bin\\
    OBJ_DIR = obj\\
    SEP = \\
else
    TARGET = bin/secure_fs
    TEST_TARGET = bin/test_runner
    MKDIR = mkdir -p obj
    MKDIR_BIN = mkdir -p bin
    RM = rm -f
    RMDIR = rm -rf
    BIN_DIR = bin/
    OBJ_DIR = obj/
    SEP = /
endif

COMMON_SRCS = src/ProcessManager.cpp \
              src/VirtualFileSystem.cpp \
              src/CapabilityManager.cpp \
              src/AuditLogger.cpp \
              src/AccessController.cpp

MAIN_SRC = src/main.cpp
TEST_SRC = tests/TestRunner.cpp

all: $(BIN_DIR)$(TARGET)

$(BIN_DIR)$(TARGET): $(COMMON_SRCS) $(MAIN_SRC)
	@$(MKDIR_BIN)
	$(CXX) $(CXXFLAGS) $(COMMON_SRCS) $(MAIN_SRC) -o $(BIN_DIR)$(TARGET)
	@echo [OK] Built $(BIN_DIR)$(TARGET) successfully.

test: $(BIN_DIR)$(TEST_TARGET)
	@echo ========================================================
	@echo Running Automated Test Suite...
	@echo ========================================================
	@$(BIN_DIR)$(TEST_TARGET)

$(BIN_DIR)$(TEST_TARGET): $(COMMON_SRCS) $(TEST_SRC)
	@$(MKDIR_BIN)
	$(CXX) $(CXXFLAGS) $(COMMON_SRCS) $(TEST_SRC) -o $(BIN_DIR)$(TEST_TARGET)
	@echo [OK] Built $(BIN_DIR)$(TEST_TARGET) successfully.

run: all
	@$(BIN_DIR)$(TARGET)

demo: all
	@$(BIN_DIR)$(TARGET) --demo

clean:
ifeq ($(OS),Windows_NT)
	@if exist bin $(RMDIR) bin
	@if exist obj $(RMDIR) obj
else
	@$(RMDIR) bin obj
endif
	@echo [OK] Cleaned build artifacts.

.PHONY: all test run demo clean
