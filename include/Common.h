#ifndef COMMON_H
#define COMMON_H

#include <string>
#include <algorithm>

//Represents the four capability-controlled operations in the system.

enum class OperationType {
    READ,
    WRITE,
    DELETE,
    EXECUTE
};

//Represents the outcome of an access control decision.
enum class AccessResult {
    ALLOWED,
    DENIED
};

// Convert OperationType enum to human-readable string.
inline std::string operationToString(OperationType op) {
    switch (op) {
        case OperationType::READ:    return "READ";
        case OperationType::WRITE:   return "WRITE";
        case OperationType::DELETE:  return "DELETE";
        case OperationType::EXECUTE: return "EXECUTE";
        default:                     return "UNKNOWN";
    }
}

// Convert human-readable string to OperationType enum.

inline bool stringToOperation(std::string str, OperationType& op) {
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
    if (str == "READ" || str == "R") {
        op = OperationType::READ;
        return true;
    } else if (str == "WRITE" || str == "W") {
        op = OperationType::WRITE;
        return true;
    } else if (str == "DELETE" || str == "DEL" || str == "D") {
        op = OperationType::DELETE;
        return true;
    } else if (str == "EXECUTE" || str == "EXEC" || str == "X") {
        op = OperationType::EXECUTE;
        return true;
    }
    return false;
}

//Convert AccessResult enum to string.
 
inline std::string accessResultToString(AccessResult res) {
    switch (res) {
        case AccessResult::ALLOWED: return "ALLOWED";
        case AccessResult::DENIED:  return "DENIED";
        default:                    return "UNKNOWN";
    }
}

#endif 
