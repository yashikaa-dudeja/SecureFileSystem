#include "AuditLogger.h"
#include <iomanip>

AuditLogger::AuditLogger() : nextLogId(1) {}

void AuditLogger::log(
    int pid, const std::string& processName, OperationType op,
    const std::string& resourcePath, AccessResult result,
    const std::string& details) {
    logEntries.emplace_back(nextLogId++, pid, processName, op, resourcePath, result, details);
}

const std::vector<AuditEntry>& AuditLogger::getLogs() const {
    return logEntries;
}

std::vector<AuditEntry> AuditLogger::getLogsForProcess(int pid) const {
    std::vector<AuditEntry> filtered;
    for (const auto& entry : logEntries) {
        if (entry.pid == pid) {
            filtered.push_back(entry);
        }
    }
    return filtered;
}

std::vector<AuditEntry> AuditLogger::getLogsForResource(const std::string& resourcePath) const {
    std::vector<AuditEntry> filtered;
    for (const auto& entry : logEntries) {
        if (entry.resourcePath == resourcePath) {
            filtered.push_back(entry);
        }
    }
    return filtered;
}

void AuditLogger::displayLogs(std::ostream& out) const {
    if (logEntries.empty()) {
        out << "\n[Audit Log is currently empty.]\n";
        return;
    }

    out << "\n"
        << "========================================================================================================\n"
        << "                                          SECURITY AUDIT LOG TRAIL\n"
        << "========================================================================================================\n";
    out << std::left
        << std::setw(5)  << "ID"
        << std::setw(20) << "Timestamp"
        << std::setw(7)  << "PID"
        << std::setw(20) << "Process"
        << std::setw(11) << "Operation"
        << std::setw(11) << "Decision"
        << std::setw(26) << "Resource"
        << "Details" << "\n";
    out << "--------------------------------------------------------------------------------------------------------\n";

    for (const auto& entry : logEntries) {
        std::string decStr = (entry.result == AccessResult::ALLOWED) ? "[ALLOWED]" : "[DENIED]";
        out << std::left
            << std::setw(5)  << entry.id
            << std::setw(20) << entry.timestamp
            << std::setw(7)  << entry.pid
            << std::setw(20) << (entry.processName.empty() ? "-" : entry.processName)
            << std::setw(11) << operationToString(entry.operation)
            << std::setw(11) << decStr
            << std::setw(26) << entry.resourcePath
            << entry.details << "\n";
    }
    out << "========================================================================================================\n";
    auto stats = getStatistics();
    out << "Total Events: " << logEntries.size()
        << " | Allowed: " << stats.first
        << " | Denied: " << stats.second << "\n\n";
}

size_t AuditLogger::getLogCount() const {
    return logEntries.size();
}

std::pair<size_t, size_t> AuditLogger::getStatistics() const {
    size_t allowed = 0;
    size_t denied = 0;
    for (const auto& entry : logEntries) {
        if (entry.result == AccessResult::ALLOWED) {
            allowed++;
        } else {
            denied++;
        }
    }
    return {allowed, denied};
}

void AuditLogger::clear() {
    logEntries.clear();
    nextLogId = 1;
}
