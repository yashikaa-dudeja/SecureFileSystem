#include "ProcessManager.h"
#include <algorithm>

ProcessManager::ProcessManager(int startingPid) : nextPid(startingPid) {}

int ProcessManager::createProcess(const std::string& name) {
    if (name.empty()) {
        return -1;
    }
    int pid = nextPid++;
    Process proc(pid, name);
    processes[pid] = proc;
    return pid;
}

bool ProcessManager::processExists(int pid) const {
    return processes.find(pid) != processes.end();
}

const Process* ProcessManager::getProcess(int pid) const {
    auto it = processes.find(pid);
    if (it != processes.end()) {
        return &(it->second);
    }
    return nullptr;
}

bool ProcessManager::terminateProcess(int pid) {
    auto it = processes.find(pid);
    if (it != processes.end()) {
        processes.erase(it);
        return true;
    }
    return false;
}

std::vector<Process> ProcessManager::getAllProcesses() const {
    std::vector<Process> list;
    list.reserve(processes.size());
    for (const auto& pair : processes) {
        list.push_back(pair.second);
    }
    // Sort by PID ascending
    std::sort(list.begin(), list.end(), [](const Process& a, const Process& b) {
        return a.pid < b.pid;
    });
    return list;
}

size_t ProcessManager::getProcessCount() const {
    return processes.size();
}

void ProcessManager::reset(int startingPid) {
    processes.clear();
    nextPid = startingPid;
}
