#ifndef CAPABILITY_MANAGER_H
#define CAPABILITY_MANAGER_H

#include <string>
#include <vector>
#include <unordered_map>
#include "Models.h"

/**
 * @brief Module 3: Capability Manager
 * Manages the capability table for the operating system simulator.
 * Each capability grants a specific process permission to perform a specific operation
 * on a specific resource.
 */
class CapabilityManager {
private:
    // Key: PID, Value: list of capabilities granted to that PID
    std::unordered_map<int, std::vector<Capability>> capabilityTable;

public:
    CapabilityManager() = default;

    /**
     * @brief Grants a capability to a process.
     * @param pid Process ID.
     * @param resourcePath Target resource path.
     * @param op Allowed operation (READ, WRITE, DELETE, EXECUTE).
     * @param outError Out parameter describing reason if grant fails.
     * @return true if granted successfully, false if duplicate or invalid.
     */
    bool grantCapability(int pid, const std::string& resourcePath, OperationType op, std::string* outError = nullptr);

    /**
     * @brief Revokes an existing capability from a process.
     * @param pid Process ID.
     * @param resourcePath Target resource path.
     * @param op Operation to revoke.
     * @param outError Out parameter describing reason if revoke fails.
     * @return true if revoked successfully, false if capability not found.
     */
    bool revokeCapability(int pid, const std::string& resourcePath, OperationType op, std::string* outError = nullptr);

    /**
     * @brief Checks if a process holds the specified capability.
     * @return true if capability exists, false otherwise.
     */
    bool hasCapability(int pid, const std::string& resourcePath, OperationType op) const;

    /**
     * @brief Retrieves all capabilities assigned to a specific process.
     */
    std::vector<Capability> getCapabilitiesForProcess(int pid) const;

    /**
     * @brief Retrieves all capabilities across all processes in the system.
     */
    std::vector<Capability> getAllCapabilities() const;

    /**
     * @brief Removes all capabilities associated with a deleted resource.
     * @return Number of revoked capabilities.
     */
    size_t removeCapabilitiesForResource(const std::string& resourcePath);

    /**
     * @brief Removes all capabilities assigned to a terminated process.
     * @return Number of revoked capabilities.
     */
    size_t removeCapabilitiesForProcess(int pid);

    /**
     * @brief Clears the entire capability table.
     */
    void clear();
};

#endif // CAPABILITY_MANAGER_H
