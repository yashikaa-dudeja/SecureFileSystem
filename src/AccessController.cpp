#include "AccessController.h"

AccessController::AccessController(
    ProcessManager& pm, VirtualFileSystem& fs,
    CapabilityManager& cm, AuditLogger& al)
    : processMgr(pm), vfs(fs), capMgr(cm), logger(al) {}

AccessResponse AccessController::requestRead(int pid, const std::string& rawPath) {
    std::string path = VirtualFileSystem::formatPath(rawPath);
    const Process* proc = processMgr.getProcess(pid);
    std::string procName = proc ? proc->name : "Unknown";

    //Process validation
    if (!proc) {
        logger.log(pid, procName, OperationType::READ, path, AccessResult::DENIED, "Invalid PID: Process not found");
        return AccessResponse(false, AccessResult::DENIED, "Error: Process with PID " + std::to_string(pid) + " does not exist.");
    }

    //Resource existence validation
    if (!vfs.exists(path)) {
        logger.log(pid, procName, OperationType::READ, path, AccessResult::DENIED, "Resource does not exist");
        return AccessResponse(false, AccessResult::DENIED, "Error: Resource '" + path + "' does not exist.");
    }

    //Capability verification
    if (!capMgr.hasCapability(pid, path, OperationType::READ)) {
        logger.log(pid, procName, OperationType::READ, path, AccessResult::DENIED, "READ capability not possessed");
        return AccessResponse(false, AccessResult::DENIED,
            "[DENIED] Process " + std::to_string(pid) + " (" + procName +
            ") does not have READ capability for '" + path + "'.");
    }

    //Authorized execution
    if (vfs.isDirectory(path)) {
        auto items = vfs.listDirectory(path);
        std::string listing = "Directory contents of " + path + ":\n";
        for (const auto& it : items) {
            listing += "  - " + it + "\n";
        }
        logger.log(pid, procName, OperationType::READ, path, AccessResult::ALLOWED, "Directory listing read");
        return AccessResponse(true, AccessResult::ALLOWED,
            "[ALLOWED] Process " + std::to_string(pid) + " read directory '" + path + "'.", listing);
    } else {
        std::string content;
        vfs.readFile(path, content);
        logger.log(pid, procName, OperationType::READ, path, AccessResult::ALLOWED,
            "File read (" + std::to_string(content.size()) + " bytes)");
        return AccessResponse(true, AccessResult::ALLOWED,
            "[ALLOWED] Process " + std::to_string(pid) + " read file '" + path + "' successfully.", content);
    }
}

AccessResponse AccessController::requestWrite(int pid, const std::string& rawPath, const std::string& content) {
    std::string path = VirtualFileSystem::formatPath(rawPath);
    const Process* proc = processMgr.getProcess(pid);
    std::string procName = proc ? proc->name : "Unknown";

    //Process validation
    if (!proc) {
        logger.log(pid, procName, OperationType::WRITE, path, AccessResult::DENIED, "Invalid PID: Process not found");
        return AccessResponse(false, AccessResult::DENIED, "Error: Process with PID " + std::to_string(pid) + " does not exist.");
    }

    //Resource existence validation
    if (!vfs.exists(path)) {
        logger.log(pid, procName, OperationType::WRITE, path, AccessResult::DENIED, "Resource does not exist");
        return AccessResponse(false, AccessResult::DENIED, "Error: Resource '" + path + "' does not exist.");
    }

    //Cannot write text directly into a directory node
    if (vfs.isDirectory(path)) {
        logger.log(pid, procName, OperationType::WRITE, path, AccessResult::DENIED, "Cannot write data to directory node");
        return AccessResponse(false, AccessResult::DENIED, "Error: Resource '" + path + "' is a directory, not a file.");
    }

    //Capability verification
    if (!capMgr.hasCapability(pid, path, OperationType::WRITE)) {
        logger.log(pid, procName, OperationType::WRITE, path, AccessResult::DENIED, "WRITE capability not possessed");
        return AccessResponse(false, AccessResult::DENIED,
            "[DENIED] Process " + std::to_string(pid) + " (" + procName +
            ") does not have WRITE capability for '" + path + "'.");
    }

    //Authorized execution
    vfs.writeFile(path, content);
    logger.log(pid, procName, OperationType::WRITE, path, AccessResult::ALLOWED,
        "Wrote " + std::to_string(content.size()) + " bytes");
    return AccessResponse(true, AccessResult::ALLOWED,
        "[ALLOWED] Process " + std::to_string(pid) + " wrote " +
        std::to_string(content.size()) + " bytes to '" + path + "' successfully.");
}

AccessResponse AccessController::requestDelete(int pid, const std::string& rawPath) {
    std::string path = VirtualFileSystem::formatPath(rawPath);
    const Process* proc = processMgr.getProcess(pid);
    std::string procName = proc ? proc->name : "Unknown";

    //Process validation
    if (!proc) {
        logger.log(pid, procName, OperationType::DELETE, path, AccessResult::DENIED, "Invalid PID: Process not found");
        return AccessResponse(false, AccessResult::DENIED, "Error: Process with PID " + std::to_string(pid) + " does not exist.");
    }

    //Resource existence validation
    if (!vfs.exists(path)) {
        logger.log(pid, procName, OperationType::DELETE, path, AccessResult::DENIED, "Resource does not exist");
        return AccessResponse(false, AccessResult::DENIED, "Error: Resource '" + path + "' does not exist.");
    }

    if (path == "/") {
        logger.log(pid, procName, OperationType::DELETE, path, AccessResult::DENIED, "Attempted to delete root");
        return AccessResponse(false, AccessResult::DENIED, "Error: Cannot delete root directory '/'.");
    }

    //Capability verification
    if (!capMgr.hasCapability(pid, path, OperationType::DELETE)) {
        logger.log(pid, procName, OperationType::DELETE, path, AccessResult::DENIED, "DELETE capability not possessed");
        return AccessResponse(false, AccessResult::DENIED,
            "[DENIED] Process " + std::to_string(pid) + " (" + procName +
            ") does not have DELETE capability for '" + path + "'.");
    }

    //Authorized execution
    vfs.deleteResource(path);
    // Cleanup any lingering capabilities for this resource
    capMgr.removeCapabilitiesForResource(path);

    logger.log(pid, procName, OperationType::DELETE, path, AccessResult::ALLOWED, "Resource deleted");
    return AccessResponse(true, AccessResult::ALLOWED,
        "[ALLOWED] Process " + std::to_string(pid) + " deleted resource '" + path + "' successfully.");
}

AccessResponse AccessController::requestExecute(int pid, const std::string& rawPath) {
    std::string path = VirtualFileSystem::formatPath(rawPath);
    const Process* proc = processMgr.getProcess(pid);
    std::string procName = proc ? proc->name : "Unknown";

    //Process validation
    if (!proc) {
        logger.log(pid, procName, OperationType::EXECUTE, path, AccessResult::DENIED, "Invalid PID: Process not found");
        return AccessResponse(false, AccessResult::DENIED, "Error: Process with PID " + std::to_string(pid) + " does not exist.");
    }

    //Resource existence validation
    if (!vfs.exists(path)) {
        logger.log(pid, procName, OperationType::EXECUTE, path, AccessResult::DENIED, "Resource does not exist");
        return AccessResponse(false, AccessResult::DENIED, "Error: Resource '" + path + "' does not exist.");
    }

    //Capability verification 
    if (!capMgr.hasCapability(pid, path, OperationType::EXECUTE)) {
        logger.log(pid, procName, OperationType::EXECUTE, path, AccessResult::DENIED, "EXECUTE capability not possessed");
        return AccessResponse(false, AccessResult::DENIED,
            "[DENIED] Process " + std::to_string(pid) + " (" + procName +
            ") does not have EXECUTE capability for '" + path + "'.");
    }

    //Safe virtual resource execution simulation
    logger.log(pid, procName, OperationType::EXECUTE, path, AccessResult::ALLOWED,
        "Simulated virtual execution successful");
    return AccessResponse(true, AccessResult::ALLOWED,
        "[EXECUTE] Process " + std::to_string(pid) + " executed virtual resource '" +
        path + "' successfully. (Virtual binary executed in isolated sandbox)");
}

bool AccessController::grantCapability(
    int pid, const std::string& rawPath, OperationType op, std::string& outMessage) {
    std::string path = VirtualFileSystem::formatPath(rawPath);
    const Process* proc = processMgr.getProcess(pid);

    if (!proc) {
        outMessage = "Grant failed: Process PID " + std::to_string(pid) + " does not exist.";
        return false;
    }

    if (!vfs.exists(path)) {
        outMessage = "Grant failed: Resource '" + path + "' does not exist in Virtual File System.";
        return false;
    }

    std::string err;
    if (!capMgr.grantCapability(pid, path, op, &err)) {
        outMessage = err;
        return false;
    }

    outMessage = "[GRANTED] Capability (" + operationToString(op) +
                 ") on '" + path + "' successfully granted to Process " +
                 std::to_string(pid) + " (" + proc->name + ").";
    return true;
}

bool AccessController::revokeCapability(
    int pid, const std::string& rawPath, OperationType op, std::string& outMessage) {
    std::string path = VirtualFileSystem::formatPath(rawPath);
    const Process* proc = processMgr.getProcess(pid);

    if (!proc) {
        outMessage = "Revocation failed: Process PID " + std::to_string(pid) + " does not exist.";
        return false;
    }

    std::string err;
    if (!capMgr.revokeCapability(pid, path, op, &err)) {
        outMessage = err;
        return false;
    }

    outMessage = "[REVOKED] Capability (" + operationToString(op) +
                 ") on '" + path + "' successfully revoked from Process " +
                 std::to_string(pid) + " (" + proc->name + ").";
    return true;
}
