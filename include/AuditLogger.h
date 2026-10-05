#ifndef AUDIT_LOGGER_H
#define AUDIT_LOGGER_H

#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include "Models.h"

/**
 * @brief Module 5: Audit Logger
 * Maintains a tamper-resistant security audit log of all resource access attempts,
 * recording both ALLOWED and DENIED requests along with timestamps and details.
 */
class AuditLogger {
private:
    std::vector<AuditEntry> logEntries;
    int nextLogId;

public:
    AuditLogger();

    /**
     * @brief Records a security event.
     */
    void log(int pid, const std::string& processName, OperationType op,
             const std::string& resourcePath, AccessResult result,
             const std::string& details = "");

    /**
     * @brief Retrieves all audit log entries.
     */
    const std::vector<AuditEntry>& getLogs() const;

    /**
     * @brief Retrieves audit entries for a specific process.
     */
    std::vector<AuditEntry> getLogsForProcess(int pid) const;

    /**
     * @brief Retrieves audit entries for a specific resource path.
     */
    std::vector<AuditEntry> getLogsForResource(const std::string& resourcePath) const;

    /**
     * @brief Pretty-prints the audit trail to standard output.
     */
    void displayLogs(std::ostream& out = std::cout) const;

    /**
     * @brief Returns total count of recorded audit entries.
     */
    size_t getLogCount() const;

    /**
     * @brief Returns counts of allowed vs denied attempts.
     */
    std::pair<size_t, size_t> getStatistics() const;

    /**
     * @brief Clears all audit log entries (used for resetting tests).
     */
    void clear();
};

#endif // AUDIT_LOGGER_H
