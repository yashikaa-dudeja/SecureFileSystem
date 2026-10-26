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


struct AccessResponse {
    bool success;               // True if operation authorized and executed
    AccessResult decision;      // ALLOWED or DENIED
    std::string message;        // Informational or error description
    std::string data;           // Content returned 

    AccessResponse(bool ok, AccessResult dec, const std::string& msg, const std::string& d = "")
        : success(ok), decision(dec), message(msg), data(d) {}
};

class AccessController {
private:
    ProcessManager& processMgr;
    VirtualFileSystem& vfs;
    CapabilityManager& capMgr;
    AuditLogger& logger;

public:
    AccessController(ProcessManager& pm, VirtualFileSystem& fs,
                     CapabilityManager& cm, AuditLogger& al);

    
    AccessResponse requestRead(int pid, const std::string& path);
    AccessResponse requestWrite(int pid, const std::string& path, const std::string& content);
    AccessResponse requestDelete(int pid, const std::string& path);
    AccessResponse requestExecute(int pid, const std::string& path);
    bool grantCapability(int pid, const std::string& path, OperationType op, std::string& outMessage);
    bool revokeCapability(int pid, const std::string& path, OperationType op, std::string& outMessage);

    // Getters for subsystems
    ProcessManager& getProcessManager() { return processMgr; }
    VirtualFileSystem& getVFS() { return vfs; }
    CapabilityManager& getCapabilityManager() { return capMgr; }
    AuditLogger& getAuditLogger() { return logger; }
};

#endif 
