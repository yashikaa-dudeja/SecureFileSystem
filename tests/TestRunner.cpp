#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include "Common.h"
#include "Models.h"
#include "ProcessManager.h"
#include "VirtualFileSystem.h"
#include "CapabilityManager.h"
#include "AuditLogger.h"
#include "AccessController.h"

static int totalTests = 0;
static int passedTests = 0;
static int failedTests = 0;

#define TEST_CASE(name) \
    do { \
        totalTests++; \
        std::cout << "[TEST " << totalTests << "] " << name << " ... "; \
    } while(0)

#define ASSERT_TRUE(condition, message) \
    do { \
        if (!(condition)) { \
            std::cout << "FAILED\n  --> Assertion failed: " << message << " (" << __FILE__ << ":" << __LINE__ << ")\n"; \
            failedTests++; \
            return; \
        } \
    } while(0)

#define ASSERT_FALSE(condition, message) ASSERT_TRUE(!(condition), message)

#define ASSERT_EQ(val1, val2, message) \
    do { \
        if ((val1) != (val2)) { \
            std::cout << "FAILED\n  --> Equality assertion failed: " << message << " (" << __FILE__ << ":" << __LINE__ << ")\n"; \
            failedTests++; \
            return; \
        } \
    } while(0)

#define TEST_PASS() \
    do { \
        passedTests++; \
        std::cout << "PASSED\n"; \
    } while(0)

// 1. Process Manager Tests
void testProcessCreation() {
    TEST_CASE("Process creation and basic attributes");
    ProcessManager pm(101);
    int p1 = pm.createProcess("ProcA");
    ASSERT_EQ(p1, 101, "First PID should be starting PID 101");
    ASSERT_TRUE(pm.processExists(101), "PID 101 must exist");
    
    const Process* proc = pm.getProcess(101);
    ASSERT_TRUE(proc != nullptr, "Process pointer must not be null");
    ASSERT_EQ(proc->name, "ProcA", "Process name must match");
    ASSERT_FALSE(proc->creationTime.empty(), "Creation timestamp must be populated");
    TEST_PASS();
}

void testUniquePIDs() {
    TEST_CASE("Unique sequential PID generation for multiple processes");
    ProcessManager pm(101);
    int p1 = pm.createProcess("Proc1");
    int p2 = pm.createProcess("Proc2");
    int p3 = pm.createProcess("Proc3");
    ASSERT_EQ(p1, 101, "PID 1");
    ASSERT_EQ(p2, 102, "PID 2");
    ASSERT_EQ(p3, 103, "PID 3");
    ASSERT_EQ(pm.getProcessCount(), 3, "Total process count should be 3");
    TEST_PASS();
}

void testInvalidPIDHandling() {
    TEST_CASE("Handling non-existent and invalid PIDs");
    ProcessManager pm(101);
    ASSERT_FALSE(pm.processExists(999), "Non-existent PID 999 should return false");
    ASSERT_FALSE(pm.processExists(-5), "Negative PID should return false");
    ASSERT_TRUE(pm.getProcess(999) == nullptr, "Pointer for non-existent PID must be nullptr");
    ASSERT_EQ(pm.createProcess(""), -1, "Empty process name should be rejected");
    TEST_PASS();
}

// 2. Virtual File System Tests
void testVFSFileCreationAndReadWrite() {
    TEST_CASE("VFS file creation, reading, and writing");
    VirtualFileSystem vfs;
    ASSERT_TRUE(vfs.createFile("/docs/test.txt", "Initial Content"), "File creation should succeed");
    ASSERT_TRUE(vfs.exists("/docs/test.txt"), "File should exist in VFS");
    ASSERT_TRUE(vfs.isFile("/docs/test.txt"), "Node must be identified as file");

    std::string content;
    ASSERT_TRUE(vfs.readFile("/docs/test.txt", content), "Reading file should succeed");
    ASSERT_EQ(content, "Initial Content", "Read content should match initial content");

    ASSERT_TRUE(vfs.writeFile("/docs/test.txt", "Overwritten Content"), "Writing file should succeed");
    ASSERT_TRUE(vfs.readFile("/docs/test.txt", content), "Reading file again");
    ASSERT_EQ(content, "Overwritten Content", "Updated content should match written data");
    TEST_PASS();
}

void testVFSDirectoryHierarchies() {
    TEST_CASE("VFS directory hierarchy creation and listing");
    VirtualFileSystem vfs;
    ASSERT_TRUE(vfs.createDirectory("/var/log/system"), "Deep directory creation should succeed");
    ASSERT_TRUE(vfs.exists("/var"), "Parent directory /var should exist");
    ASSERT_TRUE(vfs.exists("/var/log"), "Subdirectory /var/log should exist");
    ASSERT_TRUE(vfs.exists("/var/log/system"), "Deep directory /var/log/system should exist");
    ASSERT_TRUE(vfs.isDirectory("/var/log"), "Should be recognized as directory");

    ASSERT_TRUE(vfs.createFile("/var/log/system/audit.log", "Log data"), "File creation in deep dir");
    auto items = vfs.listDirectory("/var/log/system");
    ASSERT_EQ(items.size(), 1, "Directory should contain exactly 1 child item");
    ASSERT_EQ(items[0], "/var/log/system/audit.log", "Child path should match");
    TEST_PASS();
}

void testVFSResourceDeletion() {
    TEST_CASE("VFS resource deletion and subsequent lookup failure");
    VirtualFileSystem vfs;
    vfs.createFile("/tmp/file_to_delete.txt", "Delete me");
    ASSERT_TRUE(vfs.exists("/tmp/file_to_delete.txt"), "File must exist before deletion");

    ASSERT_TRUE(vfs.deleteResource("/tmp/file_to_delete.txt"), "Deletion should succeed");
    ASSERT_FALSE(vfs.exists("/tmp/file_to_delete.txt"), "File must NOT exist after deletion");

    std::string content;
    ASSERT_FALSE(vfs.readFile("/tmp/file_to_delete.txt", content), "Reading deleted file must fail");
    ASSERT_FALSE(vfs.deleteResource("/tmp/file_to_delete.txt"), "Deleting already deleted file must fail");
    ASSERT_FALSE(vfs.deleteResource("/"), "Deleting root directory '/' must be rejected");
    TEST_PASS();
}

// 3. Capability Manager Tests
void testCapabilityGrantAndCheck() {
    TEST_CASE("CapabilityManager grant, query, and process-specific isolation");
    CapabilityManager cm;
    std::string err;
    ASSERT_TRUE(cm.grantCapability(101, "/data/file.txt", OperationType::READ, &err), "Grant READ");
    ASSERT_TRUE(cm.grantCapability(101, "/data/file.txt", OperationType::WRITE, &err), "Grant WRITE");

    ASSERT_TRUE(cm.hasCapability(101, "/data/file.txt", OperationType::READ), "Process 101 must have READ");
    ASSERT_TRUE(cm.hasCapability(101, "/data/file.txt", OperationType::WRITE), "Process 101 must have WRITE");
    ASSERT_FALSE(cm.hasCapability(101, "/data/file.txt", OperationType::DELETE), "Process 101 must NOT have DELETE");

    
    ASSERT_FALSE(cm.hasCapability(102, "/data/file.txt", OperationType::READ), "Process 102 must NOT have READ");
    ASSERT_FALSE(cm.hasCapability(102, "/data/file.txt", OperationType::WRITE), "Process 102 must NOT have WRITE");
    TEST_PASS();
}

void testDuplicateCapabilityHandling() {
    TEST_CASE("Prevention of duplicate capability registration");
    CapabilityManager cm;
    std::string err;
    ASSERT_TRUE(cm.grantCapability(101, "/data/file.txt", OperationType::READ, &err), "First grant");
    ASSERT_FALSE(cm.grantCapability(101, "/data/file.txt", OperationType::READ, &err), "Duplicate grant must be rejected");
    ASSERT_FALSE(err.empty(), "Error message should describe duplicate capability");
    TEST_PASS();
}

void testCapabilityRevocation() {
    TEST_CASE("Capability revocation and invalid revocation handling");
    CapabilityManager cm;
    std::string err;
    cm.grantCapability(101, "/data/file.txt", OperationType::READ, &err);
    ASSERT_TRUE(cm.hasCapability(101, "/data/file.txt", OperationType::READ), "Capability exists");

    ASSERT_TRUE(cm.revokeCapability(101, "/data/file.txt", OperationType::READ, &err), "Revocation succeeds");
    ASSERT_FALSE(cm.hasCapability(101, "/data/file.txt", OperationType::READ), "Capability no longer held");

    // Attempting to revoke non-existent capability
    ASSERT_FALSE(cm.revokeCapability(101, "/data/file.txt", OperationType::READ, &err), "Revoking absent capability fails");
    ASSERT_FALSE(cm.revokeCapability(999, "/data/file.txt", OperationType::READ, &err), "Revoking from absent process fails");
    TEST_PASS();
}
// 4. Access Controller & Operations Tests
void testAuthorizedAndUnauthorizedRead() {
    TEST_CASE("AccessController: Authorized READ vs Unauthorized READ");
    ProcessManager pm(101);
    VirtualFileSystem vfs;
    CapabilityManager cm;
    AuditLogger al;
    AccessController ac(pm, vfs, cm, al);

    int pReader = pm.createProcess("Reader");
    int pAttacker = pm.createProcess("Attacker");
    vfs.createFile("/secure/data.txt", "Top Secret Operating Systems Research");

    std::string msg;
    ac.grantCapability(pReader, "/secure/data.txt", OperationType::READ, msg);

    // Authorized READ
    AccessResponse r1 = ac.requestRead(pReader, "/secure/data.txt");
    ASSERT_TRUE(r1.success, "Authorized READ must succeed");
    ASSERT_EQ(r1.decision, AccessResult::ALLOWED, "Decision must be ALLOWED");
    ASSERT_EQ(r1.data, "Top Secret Operating Systems Research", "Data read must match");

    // Unauthorized READ
    AccessResponse r2 = ac.requestRead(pAttacker, "/secure/data.txt");
    ASSERT_FALSE(r2.success, "Unauthorized READ must fail");
    ASSERT_EQ(r2.decision, AccessResult::DENIED, "Decision must be DENIED");
    TEST_PASS();
}

void testAuthorizedAndUnauthorizedWrite() {
    TEST_CASE("AccessController: Authorized WRITE vs Unauthorized WRITE");
    ProcessManager pm(101);
    VirtualFileSystem vfs;
    CapabilityManager cm;
    AuditLogger al;
    AccessController ac(pm, vfs, cm, al);

    int pWriter = pm.createProcess("Writer");
    int pReader = pm.createProcess("Reader");
    vfs.createFile("/docs/plan.txt", "Original Plan");

    std::string msg;
    ac.grantCapability(pWriter, "/docs/plan.txt", OperationType::WRITE, msg);
    ac.grantCapability(pReader, "/docs/plan.txt", OperationType::READ, msg);

    // Unauthorized WRITE by pReader
    AccessResponse r1 = ac.requestWrite(pReader, "/docs/plan.txt", "Tampered Content");
    ASSERT_FALSE(r1.success, "Unauthorized WRITE by pReader must be rejected");
    ASSERT_EQ(r1.decision, AccessResult::DENIED, "Decision must be DENIED");

    // Verify content did not change
    std::string curContent;
    vfs.readFile("/docs/plan.txt", curContent);
    ASSERT_EQ(curContent, "Original Plan", "File content must remain untampered");

    // Authorized WRITE by pWriter
    AccessResponse r2 = ac.requestWrite(pWriter, "/docs/plan.txt", "Authorized Plan Update");
    ASSERT_TRUE(r2.success, "Authorized WRITE by pWriter must succeed");
    ASSERT_EQ(r2.decision, AccessResult::ALLOWED, "Decision must be ALLOWED");

    // Verify content changed
    vfs.readFile("/docs/plan.txt", curContent);
    ASSERT_EQ(curContent, "Authorized Plan Update", "Content must be updated");
    TEST_PASS();
}

void testAuthorizedAndUnauthorizedDelete() {
    TEST_CASE("AccessController: Authorized DELETE vs Unauthorized DELETE");
    ProcessManager pm(101);
    VirtualFileSystem vfs;
    CapabilityManager cm;
    AuditLogger al;
    AccessController ac(pm, vfs, cm, al);

    int pAdmin = pm.createProcess("Admin");
    int pUser = pm.createProcess("User");
    vfs.createFile("/storage/temp.txt", "Temporary Data");

    std::string msg;
    ac.grantCapability(pAdmin, "/storage/temp.txt", OperationType::DELETE, msg);

    // Unauthorized DELETE by pUser
    AccessResponse r1 = ac.requestDelete(pUser, "/storage/temp.txt");
    ASSERT_FALSE(r1.success, "Unauthorized DELETE must be rejected");
    ASSERT_EQ(r1.decision, AccessResult::DENIED, "Decision must be DENIED");
    ASSERT_TRUE(vfs.exists("/storage/temp.txt"), "Resource must still exist");

    // Authorized DELETE by pAdmin
    AccessResponse r2 = ac.requestDelete(pAdmin, "/storage/temp.txt");
    ASSERT_TRUE(r2.success, "Authorized DELETE must succeed");
    ASSERT_EQ(r2.decision, AccessResult::ALLOWED, "Decision must be ALLOWED");
    ASSERT_FALSE(vfs.exists("/storage/temp.txt"), "Resource must be removed from VFS");
    TEST_PASS();
}

void testAuthorizedAndUnauthorizedExecute() {
    TEST_CASE("AccessController: Safe simulated EXECUTE operation");
    ProcessManager pm(101);
    VirtualFileSystem vfs;
    CapabilityManager cm;
    AuditLogger al;
    AccessController ac(pm, vfs, cm, al);

    int pRunner = pm.createProcess("Runner");
    int pGuest = pm.createProcess("Guest");
    vfs.createFile("/bin/simulated_kernel_module.bin", "binary image bytes");

    std::string msg;
    ac.grantCapability(pRunner, "/bin/simulated_kernel_module.bin", OperationType::EXECUTE, msg);

    // Authorized EXECUTE
    AccessResponse r1 = ac.requestExecute(pRunner, "/bin/simulated_kernel_module.bin");
    ASSERT_TRUE(r1.success, "Authorized EXECUTE must succeed");
    ASSERT_EQ(r1.decision, AccessResult::ALLOWED, "Decision must be ALLOWED");

    // Unauthorized EXECUTE
    AccessResponse r2 = ac.requestExecute(pGuest, "/bin/simulated_kernel_module.bin");
    ASSERT_FALSE(r2.success, "Unauthorized EXECUTE must be denied");
    ASSERT_EQ(r2.decision, AccessResult::DENIED, "Decision must be DENIED");
    TEST_PASS();
}

// 5. Complete Capability Revocation Lifecycle Test
void testRevocationLifecycle() {
    TEST_CASE("Crucial Requirement: Access allowed -> revoke capability -> access denied");
    ProcessManager pm(101);
    VirtualFileSystem vfs;
    CapabilityManager cm;
    AuditLogger al;
    AccessController ac(pm, vfs, cm, al);

    int pid = pm.createProcess("TestWorker");
    vfs.createFile("/config/network.cfg", "IP=192.168.1.100");

    std::string msg;
    // Step 1: Grant capability
    ASSERT_TRUE(ac.grantCapability(pid, "/config/network.cfg", OperationType::READ, msg), "Grant READ");

    // Step 2: Access succeeds
    AccessResponse respBefore = ac.requestRead(pid, "/config/network.cfg");
    ASSERT_TRUE(respBefore.success, "Access must be ALLOWED before revocation");
    ASSERT_EQ(respBefore.decision, AccessResult::ALLOWED, "Decision must be ALLOWED");

    // Step 3: Revoke capability
    ASSERT_TRUE(ac.revokeCapability(pid, "/config/network.cfg", OperationType::READ, msg), "Revoke READ");

    // Step 4: Same access attempt immediately DENIED
    AccessResponse respAfter = ac.requestRead(pid, "/config/network.cfg");
    ASSERT_FALSE(respAfter.success, "Access must be DENIED after revocation");
    ASSERT_EQ(respAfter.decision, AccessResult::DENIED, "Decision must be DENIED");
    TEST_PASS();
}

// 6. Principle of Least Privilege and Multi-Process Isolation
void testLeastPrivilegeAndIsolation() {
    TEST_CASE("Principle of Least Privilege and multi-process isolation");
    ProcessManager pm(101);
    VirtualFileSystem vfs;
    CapabilityManager cm;
    AuditLogger al;
    AccessController ac(pm, vfs, cm, al);

    int pReader = pm.createProcess("ReaderOnly");
    int pWriter = pm.createProcess("WriterOnly");
    int pZero   = pm.createProcess("ZeroPrivilege");

    vfs.createFile("/shared/document.txt", "Shared Knowledge Base");

    std::string msg;
    ac.grantCapability(pReader, "/shared/document.txt", OperationType::READ, msg);
    ac.grantCapability(pWriter, "/shared/document.txt", OperationType::WRITE, msg);

    // pReader can read, but cannot write
    ASSERT_TRUE(ac.requestRead(pReader, "/shared/document.txt").success, "pReader can read");
    ASSERT_FALSE(ac.requestWrite(pReader, "/shared/document.txt", "pReader edit").success, "pReader CANNOT write");

    // pWriter can write, but cannot read
    ASSERT_TRUE(ac.requestWrite(pWriter, "/shared/document.txt", "pWriter edit").success, "pWriter can write");
    ASSERT_FALSE(ac.requestRead(pWriter, "/shared/document.txt").success, "pWriter CANNOT read");

    // pZero cannot do either
    ASSERT_FALSE(ac.requestRead(pZero, "/shared/document.txt").success, "pZero cannot read");
    ASSERT_FALSE(ac.requestWrite(pZero, "/shared/document.txt", "Zero edit").success, "pZero cannot write");
    ASSERT_FALSE(ac.requestDelete(pZero, "/shared/document.txt").success, "pZero cannot delete");
    ASSERT_FALSE(ac.requestExecute(pZero, "/shared/document.txt").success, "pZero cannot execute");
    TEST_PASS();
}

// 7. Audit Logging Verification
void testAuditLogging() {
    TEST_CASE("AuditLogger captures both ALLOWED and DENIED events with timestamps and details");
    ProcessManager pm(101);
    VirtualFileSystem vfs;
    CapabilityManager cm;
    AuditLogger al;
    AccessController ac(pm, vfs, cm, al);

    int p1 = pm.createProcess("ClientA");
    int p2 = pm.createProcess("ClientB");
    vfs.createFile("/var/test.log", "Hello Log");

    std::string msg;
    ac.grantCapability(p1, "/var/test.log", OperationType::READ, msg);

    // 1 ALLOWED attempt
    ac.requestRead(p1, "/var/test.log");
    // 2 DENIED attempts
    ac.requestWrite(p1, "/var/test.log", "write attempt");
    ac.requestRead(p2, "/var/test.log");

    const auto& logs = al.getLogs();
    ASSERT_EQ(logs.size(), 3, "Exactly 3 audit events should be logged");

    auto stats = al.getStatistics();
    ASSERT_EQ(stats.first, 1, "Exactly 1 allowed event in statistics");
    ASSERT_EQ(stats.second, 2, "Exactly 2 denied events in statistics");

    ASSERT_EQ(logs[0].result, AccessResult::ALLOWED, "First event must be ALLOWED");
    ASSERT_EQ(logs[0].pid, p1, "First event PID must match ClientA");
    ASSERT_EQ(logs[1].result, AccessResult::DENIED, "Second event must be DENIED");
    ASSERT_EQ(logs[2].result, AccessResult::DENIED, "Third event must be DENIED");
    ASSERT_FALSE(logs[0].timestamp.empty(), "Timestamp must not be empty");
    TEST_PASS();
}

// 8. Error and Edge Case Handling
void testEdgeCasesAndValidation() {
    TEST_CASE("Edge case validation (non-existent process, non-existent resource, invalid operation)");
    ProcessManager pm(101);
    VirtualFileSystem vfs;
    CapabilityManager cm;
    AuditLogger al;
    AccessController ac(pm, vfs, cm, al);

    int p1 = pm.createProcess("ValidProc");
    vfs.createFile("/file.txt", "data");

    std::string msg;
    // Grant on non-existent process
    ASSERT_FALSE(ac.grantCapability(999, "/file.txt", OperationType::READ, msg), "Grant to non-existent process must fail");

    // Grant on non-existent file
    ASSERT_FALSE(ac.grantCapability(p1, "/ghost.txt", OperationType::READ, msg), "Grant on non-existent resource must fail");

    // Request on non-existent file
    AccessResponse rGhost = ac.requestRead(p1, "/ghost.txt");
    ASSERT_FALSE(rGhost.success, "Read on non-existent file must fail");

    // Request with non-existent process
    AccessResponse rInvProc = ac.requestRead(999, "/file.txt");
    ASSERT_FALSE(rInvProc.success, "Read with non-existent PID must fail");
    TEST_PASS();
}

int main() {
    std::cout << "\n========================================================\n";
    std::cout << "  SECURE CAPABILITY FILE SYSTEM - AUTOMATED TEST SUITE  \n";
    std::cout << "========================================================\n\n";

    // 1. Process tests
    testProcessCreation();
    testUniquePIDs();
    testInvalidPIDHandling();

    // 2. VFS tests
    testVFSFileCreationAndReadWrite();
    testVFSDirectoryHierarchies();
    testVFSResourceDeletion();

    // 3. Capability Manager tests
    testCapabilityGrantAndCheck();
    testDuplicateCapabilityHandling();
    testCapabilityRevocation();

    // 4. Access Controller tests
    testAuthorizedAndUnauthorizedRead();
    testAuthorizedAndUnauthorizedWrite();
    testAuthorizedAndUnauthorizedDelete();
    testAuthorizedAndUnauthorizedExecute();

    // 5. Revocation Lifecycle
    testRevocationLifecycle();

    // 6. Least Privilege & Multi-Process
    testLeastPrivilegeAndIsolation();

    // 7. Audit Logging
    testAuditLogging();

    // 8. Edge cases
    testEdgeCasesAndValidation();

    std::cout << "\n========================================================\n";
    std::cout << "TEST EXECUTION SUMMARY\n";
    std::cout << "========================================================\n";
    std::cout << "Total Tests Run : " << totalTests << "\n";
    std::cout << "Tests Passed    : " << passedTests << "\n";
    std::cout << "Tests Failed    : " << failedTests << "\n";

    if (failedTests == 0) {
        std::cout << "\n>>> ALL UNIT AND INTEGRATION TESTS PASSED SUCCESSFULLY! <<<\n\n";
        return 0;
    } else {
        std::cout << "\n>>> SOME TESTS FAILED. PLEASE REVIEW LOGS. <<<\n\n";
        return 1;
    }
}
