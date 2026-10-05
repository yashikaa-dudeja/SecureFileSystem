#include "CapabilityManager.h"
#include "VirtualFileSystem.h"
#include <algorithm>

bool CapabilityManager::grantCapability(
    int pid, const std::string& rawResourcePath, OperationType op, std::string* outError) {
    if (pid <= 0) {
        if (outError) *outError = "Invalid PID.";
        return false;
    }

    std::string resourcePath = VirtualFileSystem::formatPath(rawResourcePath);
    auto& list = capabilityTable[pid];

    // Check for duplicate capability
    for (const auto& cap : list) {
        if (cap.resourcePath == resourcePath && cap.operation == op) {
            if (outError) {
                *outError = "Duplicate capability: Process " + std::to_string(pid) +
                            " already holds " + operationToString(op) +
                            " for '" + resourcePath + "'.";
            }
            return false;
        }
    }

    list.emplace_back(pid, resourcePath, op);
    return true;
}

bool CapabilityManager::revokeCapability(
    int pid, const std::string& rawResourcePath, OperationType op, std::string* outError) {
    if (pid <= 0) {
        if (outError) *outError = "Invalid PID.";
        return false;
    }

    auto it = capabilityTable.find(pid);
    if (it == capabilityTable.end() || it->second.empty()) {
        if (outError) {
            *outError = "Revocation failed: Process " + std::to_string(pid) +
                        " has no capabilities assigned.";
        }
        return false;
    }

    std::string resourcePath = VirtualFileSystem::formatPath(rawResourcePath);
    auto& list = it->second;
    auto match = std::find_if(list.begin(), list.end(), [&](const Capability& c) {
        return c.resourcePath == resourcePath && c.operation == op;
    });

    if (match == list.end()) {
        if (outError) {
            *outError = "Revocation failed: Process " + std::to_string(pid) +
                        " does not possess " + operationToString(op) +
                        " on '" + resourcePath + "'.";
        }
        return false;
    }

    list.erase(match);
    return true;
}

bool CapabilityManager::hasCapability(int pid, const std::string& rawResourcePath, OperationType op) const {
    auto it = capabilityTable.find(pid);
    if (it == capabilityTable.end()) {
        return false;
    }

    std::string resourcePath = VirtualFileSystem::formatPath(rawResourcePath);
    for (const auto& cap : it->second) {
        if (cap.resourcePath == resourcePath && cap.operation == op) {
            return true;
        }
    }
    return false;
}

std::vector<Capability> CapabilityManager::getCapabilitiesForProcess(int pid) const {
    auto it = capabilityTable.find(pid);
    if (it != capabilityTable.end()) {
        return it->second;
    }
    return {};
}

std::vector<Capability> CapabilityManager::getAllCapabilities() const {
    std::vector<Capability> all;
    for (const auto& pair : capabilityTable) {
        all.insert(all.end(), pair.second.begin(), pair.second.end());
    }
    // Sort deterministically by PID then resource path
    std::sort(all.begin(), all.end(), [](const Capability& a, const Capability& b) {
        if (a.pid != b.pid) return a.pid < b.pid;
        if (a.resourcePath != b.resourcePath) return a.resourcePath < b.resourcePath;
        return static_cast<int>(a.operation) < static_cast<int>(b.operation);
    });
    return all;
}

size_t CapabilityManager::removeCapabilitiesForResource(const std::string& rawResourcePath) {
    std::string resourcePath = VirtualFileSystem::formatPath(rawResourcePath);
    size_t removedCount = 0;

    for (auto& pair : capabilityTable) {
        auto& list = pair.second;
        auto origSize = list.size();
        list.erase(
            std::remove_if(list.begin(), list.end(), [&](const Capability& c) {
                // Also match any children if directory
                return c.resourcePath == resourcePath ||
                       c.resourcePath.rfind(resourcePath + "/", 0) == 0;
            }),
            list.end());
        removedCount += (origSize - list.size());
    }
    return removedCount;
}

size_t CapabilityManager::removeCapabilitiesForProcess(int pid) {
    auto it = capabilityTable.find(pid);
    if (it != capabilityTable.end()) {
        size_t count = it->second.size();
        capabilityTable.erase(it);
        return count;
    }
    return 0;
}

void CapabilityManager::clear() {
    capabilityTable.clear();
}
