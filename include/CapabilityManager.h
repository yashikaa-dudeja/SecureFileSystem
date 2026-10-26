#ifndef CAPABILITY_MANAGER_H
#define CAPABILITY_MANAGER_H

#include <string>
#include <vector>
#include <unordered_map>
#include "Models.h"


class CapabilityManager {
private:
    // Key: PID, Value: list of capabilities granted to that PID
    std::unordered_map<int, std::vector<Capability>> capabilityTable;

public:
    CapabilityManager() = default;
    bool grantCapability(int pid, const std::string& resourcePath, OperationType op, std::string* outError = nullptr);
    bool revokeCapability(int pid, const std::string& resourcePath, OperationType op, std::string* outError = nullptr);
    bool hasCapability(int pid, const std::string& resourcePath, OperationType op) const;

    //Retrieves all capabilities associated with a specific process (PID).
     
    std::vector<Capability> getCapabilitiesForProcess(int pid) const;
    std::vector<Capability> getAllCapabilities() const;
    size_t removeCapabilitiesForResource(const std::string& resourcePath);
    size_t removeCapabilitiesForProcess(int pid);
    //remove for terminated processes
    void clear();
};

#endif 
