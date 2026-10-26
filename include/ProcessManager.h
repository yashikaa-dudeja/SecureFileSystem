#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <optional>
#include "Models.h"


class ProcessManager {
private:
    int nextPid;
    std::unordered_map<int, Process> processes;

public:
    explicit ProcessManager(int startingPid = 101);
    int createProcess(const std::string& name);
    bool processExists(int pid) const;
    const Process* getProcess(int pid) const;
    bool terminateProcess(int pid);
    std::vector<Process> getAllProcesses() const;
    size_t getProcessCount() const;

    void reset(int startingPid = 101);
};

#endif 
