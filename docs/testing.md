# Testing Strategy and Test Suite

## Overview

The automated test suite (`tests/TestRunner.cpp`) provides rigorous unit and integration verification across all modules, ensuring that no unauthorized operations bypass security and that capabilities behave according to formal capability-based operating system principles.

## Test Categories

### 1. Process Management Tests
- **Test 1**: Process creation, attributes, timestamp generation, and lookup.
- **Test 2**: Unique sequential PID allocation across multiple processes.
- **Test 3**: Invalid PID error handling (negative PIDs, non-existent PIDs, empty process names).

### 2. Virtual File System Tests
- **Test 4**: In-memory file creation, data persistence, reading, and overwriting.
- **Test 5**: Deep directory hierarchies (`/var/log/system`) and child listing.
- **Test 6**: Resource deletion, verifying that deleted nodes cannot be read or re-deleted, and root deletion protection.

### 3. Capability Manager Tests
- **Test 7**: Capability creation, querying, and verification of strict process-level isolation.
- **Test 8**: Duplicate capability detection and graceful rejection.
- **Test 9**: Capability revocation and rejection of non-existent capability revocation attempts.

### 4. Access Controller Operation Tests
- **Test 10**: Authorized READ (returns content) vs Unauthorized READ (returns DENIED).
- **Test 11**: Authorized WRITE (updates content) vs Unauthorized WRITE (rejected, original content untouched).
- **Test 12**: Authorized DELETE (removes resource) vs Unauthorized DELETE (rejected, resource preserved).
- **Test 13**: Safe simulated EXECUTE (authorized sandbox run vs unauthorized execution denial).

### 5. Revocation Lifecycle Verification
- **Test 14**: Complete dynamic lifecycle:
  $$\text{Grant capability} \longrightarrow \text{Access succeeds} \longrightarrow \text{Revoke capability} \longrightarrow \text{Same access denied}$$

### 6. Principle of Least Privilege and Multi-Process Isolation
- **Test 15**: Multi-process scenarios (ReaderOnly, WriterOnly, ZeroPrivilege) verifying that rights granted to one process never leak to another.

### 7. Security Audit Logging Tests
- **Test 16**: Audit log verification ensuring every ALLOWED and DENIED attempt is captured with timestamps and process metadata.

### 8. Edge Case Validation
- **Test 17**: Edge case resilience when querying non-existent processes, non-existent files, and invalid paths.

## Running Tests

### Using GNU Make:
```bash
make test
```

### Using Windows Batch Script:
```cmd
build.bat test
```

### Direct Compilation:
```bash
g++ -std=c++17 -Wall -Wextra -O2 -Iinclude src/ProcessManager.cpp src/VirtualFileSystem.cpp src/CapabilityManager.cpp src/AuditLogger.cpp src/AccessController.cpp tests/TestRunner.cpp -o bin/test_runner
./bin/test_runner
```
