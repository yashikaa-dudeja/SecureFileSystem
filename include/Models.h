#ifndef MODELS_H
#define MODELS_H

#include <string>
#include <vector>
#include <chrono>
#include <iomanip>
#include <sstream>
#include "Common.h"

/**
 * @brief Helper function to get current timestamp formatted as string.
 */
inline std::string getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::tm tm_buf;
#if defined(_MSC_VER) || defined(__MINGW32__) || defined(_WIN32)
    localtime_s(&tm_buf, &now_time);
#else
    localtime_r(&now_time, &tm_buf);
#endif
    std::ostringstream oss;
    oss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

/**
 * @brief Represents a simulated OS process.
 */
struct Process {
    int pid;
    std::string name;
    std::string creationTime;

    Process() : pid(-1), name(""), creationTime("") {}
    Process(int p, const std::string& n) 
        : pid(p), name(n), creationTime(getCurrentTimestamp()) {}
};

/**
 * @brief Represents a Capability: (PID + Resource + Allowed Operation).
 * In capability-based security, holding the capability is ticket to access.
 */
struct Capability {
    int pid;
    std::string resourcePath;
    OperationType operation;

    Capability() : pid(-1), resourcePath(""), operation(OperationType::READ) {}
    Capability(int p, const std::string& res, OperationType op)
        : pid(p), resourcePath(res), operation(op) {}

    bool operator==(const Capability& other) const {
        return pid == other.pid &&
               resourcePath == other.resourcePath &&
               operation == other.operation;
    }
};

/**
 * @brief Represents an audit log entry recording every access decision.
 */
struct AuditEntry {
    int id;
    std::string timestamp;
    int pid;
    std::string processName;
    OperationType operation;
    std::string resourcePath;
    AccessResult result;
    std::string details;

    AuditEntry()
        : id(0), timestamp(""), pid(-1), processName(""),
          operation(OperationType::READ), resourcePath(""),
          result(AccessResult::DENIED), details("") {}

    AuditEntry(int logId, int p, const std::string& pName, OperationType op,
               const std::string& res, AccessResult resStatus, const std::string& det = "")
        : id(logId), timestamp(getCurrentTimestamp()), pid(p), processName(pName),
          operation(op), resourcePath(res), result(resStatus), details(det) {}
};

#endif // MODELS_H
