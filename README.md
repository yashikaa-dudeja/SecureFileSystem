# Secure Capability-Based File System

**Student Name**: Yashika Dudeja  
**Registration Number**: 24BCI0260  
**Course**: Operating Systems  
**Department**: Computer Science and Engineering (Information Security), SCOPE  
**Faculty Guide**: Suchithra J  

---

## 1. Problem Statement

Traditional file systems primarily employ coarse-grained user/group permissions and permission bits (e.g., standard UNIX `rwx` bits). In complex multi-process systems, these mechanisms fail to provide fine-grained, process-level protection. A compromised process running under a privileged user account inherits all of the user's rights, violating the Principle of Least Privilege and creating vulnerability to confused-deputy attacks.

The **Secure Capability-Based File System** addresses this problem by implementing capability-based access control. Rather than querying who owns a resource, the system enforces access control by checking whether the requesting **process** explicitly possesses an unforgeable capability token authorizing the requested operation on the target resource.

---

## 2. Objectives

- **Develop an in-memory Virtual File System (VFS)** supporting files, directories, creation, reading, writing, and deletion.
- **Manage simulated processes** with unique sequential Process Identifiers (PIDs).
- **Implement a Capability Manager** supporting dynamic capability creation, storage, verification, granting, and revocation.
- **Enforce the Principle of Least Privilege** across processes using a central Reference Monitor (`AccessController`).
- **Support four capability-controlled operations**: `READ`, `WRITE`, `DELETE`, and `EXECUTE` (safe simulated execution).
- **Maintain a tamper-evident Security Audit Log** recording all authorized and unauthorized operations with timestamps.
- **Demonstrate core Operating Systems concepts**: processes, file systems, reference monitor mediation, access control, protection domains, and least privilege.

---

## 3. Project Scope

> **Academic Simulation Scope**: This project is an operating system simulator developed in modern C++17. It models process-level capability security, in-memory virtual storage, reference monitors, and audit logs. It is **not** a real Linux kernel module or hardware driver and does not execute arbitrary shell binaries on the host system.

---

## 4. Operating Systems Concepts Covered

- **Processes**: Simulated processes with unique PIDs, names, creation timestamps, and lifecycle management.
- **File Systems**: Hierarchical in-memory virtual filesystem with nodes, files, directories, and path normalization.
- **Access Control & Protection**: Capability-based security model associating subject, object, and operation.
- **Reference Monitor**: Complete mediation ensuring every operation passes through the `AccessController`.
- **System Calls**: Simulated system-call interfaces (`requestRead`, `requestWrite`, `requestDelete`, `requestExecute`).
- **Principle of Least Privilege**: Granting processes only the minimal capabilities required.
- **Dynamic Revocation**: Immediate permission revocation without stale caching.
- **Security Auditing**: Comprehensive audit trail of all access attempts.

---

## 5. System Architecture

```
Process Request (PID, Path, Op)
              |
              v
     +-----------------+
     | AccessController| <--- Central Reference Monitor
     +--------+--------+
              |
              v
     +-----------------+
     |CapabilityManager| <--- Checks (PID, Path, Op)
     +--------+--------+
             / \
            /   \
        Found   Not Found
          |         |
          v         v
        ALLOW      DENY
          |         |
       Execute    Reject
          \         /
           v       v
     +-----------------+
     |   AuditLogger   | <--- Records Event, Timestamp, Result
     +-----------------+
```

---

## 6. Core Modules

| Module | Primary Header / Implementation | Key Responsibilities |
|---|---|---|
| **Process Manager** | `include/ProcessManager.h`<br>`src/ProcessManager.cpp` | Creates processes, allocates unique sequential PIDs (101+), tracks metadata, handles process lookup and termination. |
| **Virtual File System** | `include/VirtualFileSystem.h`<br>`src/VirtualFileSystem.cpp` | Manages in-memory files and directories, path normalization, read/write storage, cascading deletion, and directory listings. |
| **Capability Manager** | `include/CapabilityManager.h`<br>`src/CapabilityManager.cpp` | Stores process-specific capability table, verifies capabilities, prevents duplicate grants, and handles dynamic revocations. |
| **Access Controller** | `include/AccessController.h`<br>`src/AccessController.cpp` | Central Reference Monitor mediating all operations (`READ`, `WRITE`, `DELETE`, `EXECUTE`), enforcing security, and notifying the audit log. |
| **Audit Logger** | `include/AuditLogger.h`<br>`src/AuditLogger.cpp` | Captures tamper-evident logs of all access attempts (ALLOWED & DENIED) with timestamps, PIDs, operations, and status. |

---

## 7. Capability Model & Operations

A capability is formally defined as:
$$\text{Capability} = (\text{Process ID}, \text{Resource Path}, \text{Operation})$$

### Operations Supported
1. **READ**: Permits reading file contents or listing directory contents.
2. **WRITE**: Permits modifying/overwriting file contents.
3. **DELETE**: Permits removing a file or directory node from the virtual namespace.
4. **EXECUTE**: Safely simulates executing a virtual binary in an isolated sandbox.

---

## 8. Technologies Used

- **Language**: Modern C++ (C++17 standard)
- **Interface**: Command-Line Interface (CLI)
- **Compiler**: GCC / G++ (MinGW on Windows, native GCC on Linux/WSL)
- **Build Tools**: GNU Make (`Makefile`), `CMakeLists.txt`, and Windows batch script (`build.bat`)
- **Libraries**: Standard C++ libraries (`<vector>`, `<unordered_map>`, `<map>`, `<memory>`, `<chrono>`, `<iomanip>`, `<sstream>`)

---

## 9. Project Structure

```
SecureFileSystem/
├── include/
│   ├── Common.h              # Enums (OperationType, AccessResult) and string helpers
│   ├── Models.h              # Data structures (Process, Capability, AuditEntry)
│   ├── ProcessManager.h      # Process management declarations
│   ├── VirtualFileSystem.h   # In-memory virtual file system declarations
│   ├── CapabilityManager.h   # Capability table & verification declarations
│   ├── AuditLogger.h         # Security audit logger declarations
│   └── AccessController.h    # Central reference monitor declarations
│
├── src/
│   ├── ProcessManager.cpp    # Process manager implementation
│   ├── VirtualFileSystem.cpp # VFS hierarchical file/dir implementation
│   ├── CapabilityManager.cpp # Capability table implementation
│   ├── AuditLogger.cpp       # Audit logging implementation
│   ├── AccessController.cpp  # Reference monitor implementation
│   └── main.cpp              # Interactive CLI & automated demonstration
│
├── tests/
│   └── TestRunner.cpp        # 17 automated unit and integration tests
│
├── docs/
│   ├── architecture.md       # Detailed architectural specifications
│   ├── capability_model.md   # Formal capability model & ACL comparison
│   ├── modules.md            # Detailed module responsibilities
│   ├── testing.md            # Comprehensive test plan and test cases
│   └── viva_walkthrough.md   # Step-by-step viva presentation guide
│
├── .gitignore                # Excludes binaries, object files, and IDE junk
├── Makefile                  # Cross-platform build script (all, test, run, demo, clean)
├── CMakeLists.txt            # CMake configuration for IDEs
├── build.bat                 # One-click Windows build and test batch file
└── README.md                 # Complete project documentation
```

---

## 10. How to Compile and Run

### Option A: Using GNU Make (Recommended on Linux, WSL, or MinGW)

```bash
# 1. Compile the main application
make

# 2. Run the interactive CLI
make run

# 3. Run the automated live demonstration
make demo

# 4. Compile and run the automated test suite
make test

# 5. Clean build artifacts
make clean
```

### Option B: Using Windows Batch Script (`build.bat`)

```cmd
:: Build the main application
build.bat

:: Run the interactive CLI
build.bat run

:: Run the automated demonstration
build.bat demo

:: Compile and run all automated tests
build.bat test

:: Clean build artifacts
build.bat clean
```

### Option C: Direct G++ Compilation

```bash
# Compile CLI application
g++ -std=c++17 -Wall -Wextra -O2 -Iinclude src/ProcessManager.cpp src/VirtualFileSystem.cpp src/CapabilityManager.cpp src/AuditLogger.cpp src/AccessController.cpp src/main.cpp -o bin/secure_fs

# Compile Automated Test Suite
g++ -std=c++17 -Wall -Wextra -O2 -Iinclude src/ProcessManager.cpp src/VirtualFileSystem.cpp src/CapabilityManager.cpp src/AuditLogger.cpp src/AccessController.cpp tests/TestRunner.cpp -o bin/test_runner
```

---

## 11. Command-Line Interface Menu

When started, the application displays:

```
========================================
   Secure Capability-Based File System  
========================================
 1. Create Process
 2. List Processes
 3. Create File
 4. Create Directory
 5. List Resources
 6. Grant Capability
 7. Revoke Capability
 8. List Capabilities
 9. Read File
10. Write File
11. Delete Resource
12. Execute Resource
13. View Audit Log
14. Run Demo
 0. Exit
========================================
```

---

## 12. Security Demonstrations

### Example 1: Authorized Operation
- Process `101` (`ReaderProcess`) holds `READ` capability on `/documents/report.txt`.
- Request: `READ /documents/report.txt`
- Result: **`[ALLOWED]`** — Content is retrieved and displayed. Logged as `ALLOWED`.

### Example 2: Unauthorized Operation (Least Privilege)
- Process `101` (`ReaderProcess`) attempts `WRITE /documents/report.txt`.
- Result: **`[DENIED]`** — Process 101 lacks `WRITE` capability. Request rejected and logged as `DENIED`.

### Example 3: Dynamic Capability Revocation
- Grant `WRITE` on `/documents/report.txt` to Process `101`.
- Write attempt $\rightarrow$ **`[ALLOWED]`**.
- Revoke `WRITE` on `/documents/report.txt` from Process `101`.
- Immediate write attempt $\rightarrow$ **`[DENIED]`**.
- Demonstrates immediate revocation enforcement without caching latency.

### Example 4: Audit Log Inspection
Executing Option `13` displays the audit trail:

```text
========================================================================================================
                                          SECURITY AUDIT LOG TRAIL
========================================================================================================
ID   Timestamp           PID    Process             Operation  Decision   Resource                  Details
--------------------------------------------------------------------------------------------------------
1    2026-10-05 21:40:28 101    ReaderProcess       READ       [ALLOWED]  /documents/report.txt     File read (56 bytes)
2    2026-10-05 21:40:28 101    ReaderProcess       WRITE      [DENIED]   /documents/report.txt     WRITE capability not possessed
3    2026-10-05 21:40:28 103    RestrictedProcess   READ       [DENIED]   /documents/report.txt     READ capability not possessed
4    2026-10-05 21:40:28 101    ReaderProcess       WRITE      [ALLOWED]  /documents/report.txt     Wrote 38 bytes
5    2026-10-05 21:40:28 101    ReaderProcess       WRITE      [DENIED]   /documents/report.txt     WRITE capability not possessed
6    2026-10-05 21:40:28 101    ReaderProcess       EXECUTE    [ALLOWED]  /bin/backup_daemon.bin    Simulated virtual execution successful
7    2026-10-05 21:40:28 102    WriterProcess       EXECUTE    [DENIED]   /bin/backup_daemon.bin    EXECUTE capability not possessed
========================================================================================================
Total Events: 7 | Allowed: 3 | Denied: 4
```

---

## 13. Automated Test Suite Results

The automated test runner executes 17 test cases covering all modules and requirements:

```text
========================================================
  SECURE CAPABILITY FILE SYSTEM - AUTOMATED TEST SUITE  
========================================================

[TEST 1] Process creation and basic attributes ... PASSED
[TEST 2] Unique sequential PID generation for multiple processes ... PASSED
[TEST 3] Handling non-existent and invalid PIDs ... PASSED
[TEST 4] VFS file creation, reading, and writing ... PASSED
[TEST 5] VFS directory hierarchy creation and listing ... PASSED
[TEST 6] VFS resource deletion and subsequent lookup failure ... PASSED
[TEST 7] CapabilityManager grant, query, and process-specific isolation ... PASSED
[TEST 8] Prevention of duplicate capability registration ... PASSED
[TEST 9] Capability revocation and invalid revocation handling ... PASSED
[TEST 10] AccessController: Authorized READ vs Unauthorized READ ... PASSED
[TEST 11] AccessController: Authorized WRITE vs Unauthorized WRITE ... PASSED
[TEST 12] AccessController: Authorized DELETE vs Unauthorized DELETE ... PASSED
[TEST 13] AccessController: Safe simulated EXECUTE operation ... PASSED
[TEST 14] Crucial Requirement: Access allowed -> revoke capability -> access denied ... PASSED
[TEST 15] Principle of Least Privilege and multi-process isolation ... PASSED
[TEST 16] AuditLogger captures both ALLOWED and DENIED events with timestamps and details ... PASSED
[TEST 17] Edge case validation (non-existent process, non-existent resource, invalid operation) ... PASSED

========================================================
TEST EXECUTION SUMMARY
========================================================
Total Tests Run : 17
Tests Passed    : 17
Tests Failed    : 0

>>> ALL UNIT AND INTEGRATION TESTS PASSED SUCCESSFULLY! <<<
```

---

## 14. Limitations and Future Improvements

### Current Limitations
- **In-Memory Storage**: Virtual file system exists in program memory and resets between process runs.
- **Single Process Host Runtime**: The simulation models multiple processes logically within a single runtime rather than POSIX `fork()`/`exec()`.
- **Capability Delegation**: Capabilities are granted administratively; capability transfer between processes is not implemented.

### Future Improvements
- **Persistent Disk Serialization**: Save/load VFS trees and capability tables to/from encrypted files on disk.
- **Capability Delegation and Confinement**: Support processes transferring capabilities with attenuated rights.
- **Cryptographic Capabilities**: Implement cryptographically signed capability tokens (e.g. HMAC or Amoeba OS style spars).
