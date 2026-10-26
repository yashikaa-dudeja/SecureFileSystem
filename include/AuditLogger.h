#ifndef AUDIT_LOGGER_H
#define AUDIT_LOGGER_H

#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include "Models.h"

/*
Maintains a tamper-resistant security audit log of all resource access attempts
 */
class AuditLogger {
private:
    std::vector<AuditEntry> logEntries;
    int nextLogId;

public:
    AuditLogger();

    //Records a security event.
    void log(int pid, const std::string& processName, OperationType op,
             const std::string& resourcePath, AccessResult result,
             const std::string& details = "");

    //Retrieves all audit log entries.
    const std::vector<AuditEntry>& getLogs() const;

    //Retrieves audit entries for a specific process.
    std::vector<AuditEntry> getLogsForProcess(int pid) const;

    //Retrieves audit entries for a specific resource path.
    std::vector<AuditEntry> getLogsForResource(const std::string& resourcePath) const;

    void displayLogs(std::ostream& out = std::cout) const;
    size_t getLogCount() const;
    std::pair<size_t, size_t> getStatistics() const;
    void clear();
};

#endif // AUDIT_LOGGER_H
