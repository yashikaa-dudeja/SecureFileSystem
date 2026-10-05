#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <optional>
#include "Models.h"

/**
 * @brief Module 1: Process Manager
 * Responsible for creating simulated processes, generating unique PIDs,
 * querying process details, and maintaining active process table.
 */
class ProcessManager {
private:
    int nextPid;
    std::unordered_map<int, Process> processes;

public:
    explicit ProcessManager(int startingPid = 101);

    /**
     * @brief Creates a new simulated process with a unique PID.
     * @param name Name of the process.
     * @return Newly assigned unique PID, or -1 on failure.
     */
    int createProcess(const std::string& name);

    /**
     * @brief Checks if a process with given PID exists.
     */
    bool processExists(int pid) const;

    /**
     * @brief Retrieves process information by PID.
     * @return Pointer to Process if found, nullptr otherwise.
     */
    const Process* getProcess(int pid) const;

    /**
     * @brief Terminates/removes a simulated process.
     * @return true if process was found and removed, false otherwise.
     */
    bool terminateProcess(int pid);

    /**
     * @brief Returns a list of all active processes.
     */
    std::vector<Process> getAllProcesses() const;

    /**
     * @brief Returns the total count of active processes.
     */
    size_t getProcessCount() const;

    /**
     * @brief Resets process table and PID counter (useful for tests/demos).
     */
    void reset(int startingPid = 101);
};

#endif // PROCESS_MANAGER_H
