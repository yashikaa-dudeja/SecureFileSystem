#ifndef ACCESS_CONTROLLER_H
#define ACCESS_CONTROLLER_H

#include <string>
#include <memory>
#include "Common.h"
#include "Models.h"
#include "ProcessManager.h"
#include "VirtualFileSystem.h"
#include "CapabilityManager.h"
#include "AuditLogger.h"

/**
 * @brief Result struct returned by AccessController system calls.
 */
struct AccessResponse {
    bool success;               // True if operation authorized and executed
    AccessResult decision;      // ALLOWED or DENIED
    std::string message;        // Informational or error description
    std::string data;           // Content returned (e.g. for READ)

    AccessResponse(bool ok, AccessResult dec, const std::string& msg, const std::string& d = "")
        : success(ok), decision(dec), message(msg), data(d) {}
};

/**
 * @brief Module 4: Access Controller
 * Central Reference Monitor enforcing the capability-based security model.
 * All process operations (READ, WRITE, DELETE, EXECUTE) MUST pass through this
 * controller before reaching the Virtual File System.
 */
class AccessController {
private:
    ProcessManager& processMgr;
    VirtualFileSystem& vfs;
    CapabilityManager& capMgr;
    AuditLogger& logger;

public:
    AccessController(ProcessManager& pm, VirtualFileSystem& fs,
                     CapabilityManager& cm, AuditLogger& al);

    /**
     * @brief System-call style READ request.
     * Process requests READ on resource -> Access Controller checks capability
     * -> ALLOW/DENY -> Read from VFS or reject -> Audit Log.
     */
    AccessResponse requestRead(int pid, const std::string& path);

    /**
     * @brief System-call style WRITE request.
     * Process requests WRITE on resource -> Access Controller checks capability
     * -> ALLOW/DENY -> Write to VFS or reject -> Audit Log.
     */
    AccessResponse requestWrite(int pid, const std::string& path, const std::string& content);

    /**
     * @brief System-call style DELETE request.
     * Process requests DELETE on resource -> Access Controller checks capability
     * -> ALLOW/DENY -> Delete from VFS & revoke stale capabilities or reject -> Audit Log.
     */
    AccessResponse requestDelete(int pid, const std::string& path);

    /**
     * @brief System-call style EXECUTE request.
     * Process requests EXECUTE on virtual resource -> Access Controller checks capability
     * -> ALLOW/DENY -> Safe virtual execution simulation -> Audit Log.
     */
    AccessResponse requestExecute(int pid, const std::string& path);

    /**
     * @brief Dynamic capability grant.
     * Validates process and resource exist, then registers the capability.
     */
    bool grantCapability(int pid, const std::string& path, OperationType op, std::string& outMessage);

    /**
     * @brief Dynamic capability revocation.
     * Revokes capability and ensures subsequent requests are immediately denied.
     */
    bool revokeCapability(int pid, const std::string& path, OperationType op, std::string& outMessage);

    // Getters for subsystems
    ProcessManager& getProcessManager() { return processMgr; }
    VirtualFileSystem& getVFS() { return vfs; }
    CapabilityManager& getCapabilityManager() { return capMgr; }
    AuditLogger& getAuditLogger() { return logger; }
};

#endif // ACCESS_CONTROLLER_H
