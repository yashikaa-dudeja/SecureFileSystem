#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include "Common.h"
#include "Models.h"
#include "ProcessManager.h"
#include "VirtualFileSystem.h"
#include "CapabilityManager.h"
#include "AuditLogger.h"
#include "AccessController.h"

// Helper function to safely read an integer from stdin
static bool readInt(int& value) {
    if (std::cin >> value) {
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return true;
    }
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return false;
}

// Helper function to safely read a full line of string
static std::string readLine(const std::string& prompt) {
    std::cout << prompt;
    std::string line;
    std::getline(std::cin, line);
    return line;
}

// Interactive Automated Demonstration
void runDemonstration(AccessController& controller) {
    std::cout << "\n========================================================================\n";
    std::cout << "          AUTOMATED LIVE SECURITY DEMONSTRATION WORKFLOW\n";
    std::cout << "========================================================================\n";
    std::cout << "Demonstrating Principle of Least Privilege, Dynamic Grant & Revoke,\n";
    std::cout << "Process Isolation, Safe Execution, and Security Audit Logging.\n\n";

    ProcessManager& pm = controller.getProcessManager();
    VirtualFileSystem& vfs = controller.getVFS();
    CapabilityManager& cm = controller.getCapabilityManager();
    AuditLogger& logger = controller.getAuditLogger();

    // Reset simulator state for pristine demo
    pm.reset(101);
    vfs.clear();
    cm.clear();
    logger.clear();

    // Step 1: Create Processes
    std::cout << "[Step 1] Creating Simulated Processes...\n";
    int pidReader = pm.createProcess("ReaderProcess");
    int pidWriter = pm.createProcess("WriterProcess");
    int pidRestricted = pm.createProcess("RestrictedProcess");

    std::cout << "  - Process Created: PID " << pidReader << " (ReaderProcess)\n";
    std::cout << "  - Process Created: PID " << pidWriter << " (WriterProcess)\n";
    std::cout << "  - Process Created: PID " << pidRestricted << " (RestrictedProcess)\n\n";

    // Step 2: Create Virtual File System Resources
    std::cout << "[Step 2] Initializing Virtual File System Resources...\n";
    vfs.createDirectory("/documents");
    vfs.createDirectory("/bin");
    vfs.createFile("/documents/report.txt", "CONFIDENTIAL: Operating Systems Capability Model Report.");
    vfs.createFile("/bin/backup_daemon.bin", "# Virtual Executable Binary");

    std::cout << "  - Directory Created: /documents\n";
    std::cout << "  - Directory Created: /bin\n";
    std::cout << "  - Virtual File Created: /documents/report.txt\n";
    std::cout << "  - Virtual Executable Created: /bin/backup_daemon.bin\n\n";

    // Step 3: Grant Initial Capabilities (Least Privilege)
    std::cout << "[Step 3] Granting Capabilities (Enforcing Principle of Least Privilege)...\n";
    std::string msg;
    controller.grantCapability(pidReader, "/documents/report.txt", OperationType::READ, msg);
    std::cout << "  " << msg << "\n";

    controller.grantCapability(pidWriter, "/documents/report.txt", OperationType::WRITE, msg);
    std::cout << "  " << msg << "\n";

    controller.grantCapability(pidReader, "/bin/backup_daemon.bin", OperationType::EXECUTE, msg);
    std::cout << "  " << msg << "\n";
    std::cout << "  * Notice: Process " << pidRestricted << " has NOT been granted any capabilities.\n\n";

    // Step 4: Authorized READ
    std::cout << "[Step 4] Process " << pidReader << " requests READ on '/documents/report.txt'...\n";
    AccessResponse resp = controller.requestRead(pidReader, "/documents/report.txt");
    std::cout << "  Decision: " << (resp.decision == AccessResult::ALLOWED ? "ALLOWED" : "DENIED") << "\n";
    std::cout << "  Status Message: " << resp.message << "\n";
    std::cout << "  File Content: \"" << resp.data << "\"\n\n";

    // Step 5: Unauthorized Access Attempts
    std::cout << "[Step 5] Testing Unauthorized Operations (Process-Level Isolation)...\n";
    std::cout << "  (A) Process " << pidReader << " attempts unauthorized WRITE on '/documents/report.txt'...\n";
    resp = controller.requestWrite(pidReader, "/documents/report.txt", "Hacked Content!");
    std::cout << "      Decision: " << (resp.decision == AccessResult::ALLOWED ? "ALLOWED" : "DENIED") << "\n";
    std::cout << "      Status Message: " << resp.message << "\n";

    std::cout << "  (B) Process " << pidRestricted << " attempts unauthorized READ on '/documents/report.txt'...\n";
    resp = controller.requestRead(pidRestricted, "/documents/report.txt");
    std::cout << "      Decision: " << (resp.decision == AccessResult::ALLOWED ? "ALLOWED" : "DENIED") << "\n";
    std::cout << "      Status Message: " << resp.message << "\n\n";

    // Step 6: Dynamic Capability Grant & Verification
    std::cout << "[Step 6] Dynamic Capability Grant...\n";
    std::cout << "  Granting WRITE capability on '/documents/report.txt' to Process " << pidReader << "...\n";
    controller.grantCapability(pidReader, "/documents/report.txt", OperationType::WRITE, msg);
    std::cout << "  " << msg << "\n";

    std::cout << "  Process " << pidReader << " re-attempts WRITE on '/documents/report.txt'...\n";
    resp = controller.requestWrite(pidReader, "/documents/report.txt", "Updated report with authorized append.");
    std::cout << "  Decision: " << (resp.decision == AccessResult::ALLOWED ? "ALLOWED" : "DENIED") << "\n";
    std::cout << "  Status Message: " << resp.message << "\n\n";

    // Step 7: Dynamic Capability Revocation & Immediate Denial
    std::cout << "[Step 7] Dynamic Capability Revocation (Crucial OS Demonstration)...\n";
    std::cout << "  Revoking WRITE capability on '/documents/report.txt' from Process " << pidReader << "...\n";
    controller.revokeCapability(pidReader, "/documents/report.txt", OperationType::WRITE, msg);
    std::cout << "  " << msg << "\n";

    std::cout << "  Process " << pidReader << " immediately attempts WRITE again after revocation...\n";
    resp = controller.requestWrite(pidReader, "/documents/report.txt", "Another write attempt.");
    std::cout << "  Decision: " << (resp.decision == AccessResult::ALLOWED ? "ALLOWED" : "DENIED") << "\n";
    std::cout << "  Status Message: " << resp.message << "\n";
    std::cout << "  * Confirmation: Access allowed before revocation -> revoked -> access immediately denied!\n\n";

    // Step 8: Safe EXECUTE Operation
    std::cout << "[Step 8] Safe Virtual Resource Execution Demonstration...\n";
    std::cout << "  (A) Process " << pidReader << " requests EXECUTE on '/bin/backup_daemon.bin'...\n";
    resp = controller.requestExecute(pidReader, "/bin/backup_daemon.bin");
    std::cout << "      Decision: " << (resp.decision == AccessResult::ALLOWED ? "ALLOWED" : "DENIED") << "\n";
    std::cout << "      Status Message: " << resp.message << "\n";

    std::cout << "  (B) Process " << pidWriter << " requests unauthorized EXECUTE on '/bin/backup_daemon.bin'...\n";
    resp = controller.requestExecute(pidWriter, "/bin/backup_daemon.bin");
    std::cout << "      Decision: " << (resp.decision == AccessResult::ALLOWED ? "ALLOWED" : "DENIED") << "\n";
    std::cout << "      Status Message: " << resp.message << "\n\n";

    // Step 9: Audit Trail Inspection
    std::cout << "[Step 9] Security Audit Log Trail Review...\n";
    logger.displayLogs();

    std::cout << "========================================================================\n";
    std::cout << "                DEMONSTRATION COMPLETED SUCCESSFULLY\n";
    std::cout << "========================================================================\n\n";
}

int main(int argc, char* argv[]) {
    ProcessManager processMgr(101);
    VirtualFileSystem vfs;
    CapabilityManager capMgr;
    AuditLogger logger;
    AccessController controller(processMgr, vfs, capMgr, logger);

    if (argc > 1 && std::string(argv[1]) == "--demo") {
        runDemonstration(controller);
        return 0;
    }

    // Default initialization with sample environment
    int p1 = processMgr.createProcess("InitProcess");
    vfs.createDirectory("/documents");
    vfs.createFile("/documents/sample.txt", "Welcome to Secure Capability-Based File System.");
    std::string initMsg;
    controller.grantCapability(p1, "/documents/sample.txt", OperationType::READ, initMsg);

    while (true) {
        std::cout << "========================================\n";
        std::cout << "   Secure Capability-Based File System  \n";
        std::cout << "========================================\n";
        std::cout << " 1. Create Process\n";
        std::cout << " 2. List Processes\n";
        std::cout << " 3. Create File\n";
        std::cout << " 4. Create Directory\n";
        std::cout << " 5. List Resources\n";
        std::cout << " 6. Grant Capability\n";
        std::cout << " 7. Revoke Capability\n";
        std::cout << " 8. List Capabilities\n";
        std::cout << " 9. Read File\n";
        std::cout << "10. Write File\n";
        std::cout << "11. Delete Resource\n";
        std::cout << "12. Execute Resource\n";
        std::cout << "13. View Audit Log\n";
        std::cout << "14. Run Demo\n";
        std::cout << " 0. Exit\n";
        std::cout << "========================================\n";
        std::cout << "Enter choice: ";

        int choice = -1;
        if (!readInt(choice)) {
            std::cout << "\n[!] Invalid input. Please enter a numerical choice.\n\n";
            continue;
        }

        if (choice == 0) {
            std::cout << "\nExiting Secure Capability File System simulator. Goodbye!\n";
            break;
        }

        switch (choice) {
            case 1: { // Create Process
                std::string name = readLine("Enter process name: ");
                if (name.empty()) {
                    std::cout << "[!] Process name cannot be empty.\n\n";
                    break;
                }
                int pid = processMgr.createProcess(name);
                std::cout << "[+] Process created successfully. PID: " << pid
                          << " | Name: " << name << "\n\n";
                break;
            }
            case 2: { // List Processes
                auto list = processMgr.getAllProcesses();
                std::cout << "\nActive Processes (" << list.size() << "):\n";
                std::cout << "----------------------------------------\n";
                if (list.empty()) {
                    std::cout << "  (No active processes)\n";
                } else {
                    for (const auto& proc : list) {
                        std::cout << "  PID: " << proc.pid
                                  << " | Name: " << proc.name
                                  << " | Created: " << proc.creationTime << "\n";
                    }
                }
                std::cout << "----------------------------------------\n\n";
                break;
            }
            case 3: { // Create File
                std::string path = readLine("Enter virtual file path (e.g. /documents/notes.txt): ");
                std::string content = readLine("Enter initial content (optional): ");
                if (vfs.createFile(path, content)) {
                    std::cout << "[+] File created successfully: " << VirtualFileSystem::formatPath(path) << "\n\n";
                } else {
                    std::cout << "[-] Failed to create file. Resource may already exist or path is invalid.\n\n";
                }
                break;
            }
            case 4: { // Create Directory
                std::string path = readLine("Enter virtual directory path (e.g. /projects/os): ");
                if (vfs.createDirectory(path)) {
                    std::cout << "[+] Directory created successfully: " << VirtualFileSystem::formatPath(path) << "\n\n";
                } else {
                    std::cout << "[-] Failed to create directory.\n\n";
                }
                break;
            }
            case 5: { // List Resources
                auto resources = vfs.listAllResources();
                std::cout << "\nVirtual File System Resources (" << resources.size() << "):\n";
                std::cout << "----------------------------------------\n";
                if (resources.empty()) {
                    std::cout << "  (File system is empty)\n";
                } else {
                    for (const auto& res : resources) {
                        std::cout << "  " << res << "\n";
                    }
                }
                std::cout << "----------------------------------------\n\n";
                break;
            }
            case 6: { // Grant Capability
                std::cout << "Enter target Process PID: ";
                int pid;
                if (!readInt(pid)) {
                    std::cout << "[!] Invalid PID format.\n\n";
                    break;
                }
                std::string path = readLine("Enter resource path: ");
                std::string opStr = readLine("Enter operation (READ, WRITE, DELETE, EXECUTE): ");
                OperationType op;
                if (!stringToOperation(opStr, op)) {
                    std::cout << "[!] Invalid operation. Valid: READ, WRITE, DELETE, EXECUTE.\n\n";
                    break;
                }
                std::string outMsg;
                if (controller.grantCapability(pid, path, op, outMsg)) {
                    std::cout << "[+] " << outMsg << "\n\n";
                } else {
                    std::cout << "[-] " << outMsg << "\n\n";
                }
                break;
            }
            case 7: { // Revoke Capability
                std::cout << "Enter target Process PID: ";
                int pid;
                if (!readInt(pid)) {
                    std::cout << "[!] Invalid PID format.\n\n";
                    break;
                }
                std::string path = readLine("Enter resource path: ");
                std::string opStr = readLine("Enter operation to revoke (READ, WRITE, DELETE, EXECUTE): ");
                OperationType op;
                if (!stringToOperation(opStr, op)) {
                    std::cout << "[!] Invalid operation.\n\n";
                    break;
                }
                std::string outMsg;
                if (controller.revokeCapability(pid, path, op, outMsg)) {
                    std::cout << "[+] " << outMsg << "\n\n";
                } else {
                    std::cout << "[-] " << outMsg << "\n\n";
                }
                break;
            }
            case 8: { // List Capabilities
                auto allCaps = capMgr.getAllCapabilities();
                std::cout << "\nCapability Table (" << allCaps.size() << " active capabilities):\n";
                std::cout << "------------------------------------------------------------\n";
                std::cout << std::left
                          << std::setw(8)  << "PID"
                          << std::setw(18) << "Process Name"
                          << std::setw(12) << "Operation"
                          << "Resource\n";
                std::cout << "------------------------------------------------------------\n";
                if (allCaps.empty()) {
                    std::cout << "  (No capabilities assigned)\n";
                } else {
                    for (const auto& cap : allCaps) {
                        const Process* p = processMgr.getProcess(cap.pid);
                        std::string pName = p ? p->name : "Terminated";
                        std::cout << std::left
                                  << std::setw(8)  << cap.pid
                                  << std::setw(18) << pName
                                  << std::setw(12) << operationToString(cap.operation)
                                  << cap.resourcePath << "\n";
                    }
                }
                std::cout << "------------------------------------------------------------\n\n";
                break;
            }
            case 9: { // Read File
                std::cout << "Enter requesting Process PID: ";
                int pid;
                if (!readInt(pid)) {
                    std::cout << "[!] Invalid PID format.\n\n";
                    break;
                }
                std::string path = readLine("Enter file path: ");
                AccessResponse resp = controller.requestRead(pid, path);
                std::cout << "\n>>> " << resp.message << "\n";
                if (resp.success) {
                    std::cout << "----------------------------------------\n";
                    std::cout << resp.data << "\n";
                    std::cout << "----------------------------------------\n";
                }
                std::cout << "\n";
                break;
            }
            case 10: { // Write File
                std::cout << "Enter requesting Process PID: ";
                int pid;
                if (!readInt(pid)) {
                    std::cout << "[!] Invalid PID format.\n\n";
                    break;
                }
                std::string path = readLine("Enter file path: ");
                std::string content = readLine("Enter content to write: ");
                AccessResponse resp = controller.requestWrite(pid, path, content);
                std::cout << "\n>>> " << resp.message << "\n\n";
                break;
            }
            case 11: { // Delete Resource
                std::cout << "Enter requesting Process PID: ";
                int pid;
                if (!readInt(pid)) {
                    std::cout << "[!] Invalid PID format.\n\n";
                    break;
                }
                std::string path = readLine("Enter resource path to delete: ");
                AccessResponse resp = controller.requestDelete(pid, path);
                std::cout << "\n>>> " << resp.message << "\n\n";
                break;
            }
            case 12: { // Execute Resource
                std::cout << "Enter requesting Process PID: ";
                int pid;
                if (!readInt(pid)) {
                    std::cout << "[!] Invalid PID format.\n\n";
                    break;
                }
                std::string path = readLine("Enter virtual resource path to execute: ");
                AccessResponse resp = controller.requestExecute(pid, path);
                std::cout << "\n>>> " << resp.message << "\n\n";
                break;
            }
            case 13: { // View Audit Log
                logger.displayLogs();
                break;
            }
            case 14: { // Run Demo
                runDemonstration(controller);
                break;
            }
            default: {
                std::cout << "[!] Unknown option (" << choice << "). Please select 0-14.\n\n";
                break;
            }
        }
    }

    return 0;
}
