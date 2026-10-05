#ifndef VIRTUAL_FILE_SYSTEM_H
#define VIRTUAL_FILE_SYSTEM_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>

enum class NodeType {
    FILE_NODE,
    DIRECTORY_NODE
};

/**
 * @brief Base node in the Virtual File System tree.
 */
class VFSNode {
public:
    std::string name;
    std::string fullPath;
    NodeType type;
    std::string createdAt;
    std::string modifiedAt;

    VFSNode(const std::string& n, const std::string& path, NodeType t);
    virtual ~VFSNode() = default;
};

/**
 * @brief Virtual file holding text or binary-simulated content.
 */
class VFSFile : public VFSNode {
public:
    std::string content;

    VFSFile(const std::string& n, const std::string& path, const std::string& initialContent = "");
    void write(const std::string& newContent);
    size_t getSize() const;
};

/**
 * @brief Virtual directory holding child files and subdirectories.
 */
class VFSDirectory : public VFSNode {
public:
    // Sorted map for deterministic order
    std::map<std::string, std::shared_ptr<VFSNode>> children;

    VFSDirectory(const std::string& n, const std::string& path);
};

/**
 * @brief Module 2: Virtual File System
 * Manages an in-memory hierarchical file system with files and directories.
 */
class VirtualFileSystem {
private:
    std::shared_ptr<VFSDirectory> root;

    // Helper utilities
    static std::string normalizePath(const std::string& rawPath);
    static std::vector<std::string> splitPath(const std::string& normalizedPath);
    std::shared_ptr<VFSNode> findNode(const std::string& normalizedPath) const;
    std::shared_ptr<VFSDirectory> findParentDirectory(const std::string& normalizedPath, std::string& outBaseName) const;
    void collectAllResources(const std::shared_ptr<VFSDirectory>& dir, std::vector<std::string>& outList) const;

public:
    VirtualFileSystem();
    ~VirtualFileSystem() = default;

    /**
     * @brief Creates a directory at the given path.
     * Recursively creates parent directories if needed.
     */
    bool createDirectory(const std::string& path);

    /**
     * @brief Creates a new file at the given path with optional initial content.
     */
    bool createFile(const std::string& path, const std::string& initialContent = "");

    /**
     * @brief Checks if a resource (file or directory) exists at path.
     */
    bool exists(const std::string& path) const;

    /**
     * @brief Checks if a resource is a file.
     */
    bool isFile(const std::string& path) const;

    /**
     * @brief Checks if a resource is a directory.
     */
    bool isDirectory(const std::string& path) const;

    /**
     * @brief Reads content of a file. Returns false if not found or is a directory.
     */
    bool readFile(const std::string& path, std::string& outContent) const;

    /**
     * @brief Writes content to a file (overwrites). Returns false if not found or is a directory.
     */
    bool writeFile(const std::string& path, const std::string& content);

    /**
     * @brief Deletes a file or directory (and its children). Root cannot be deleted.
     */
    bool deleteResource(const std::string& path);

    /**
     * @brief Lists all children in a specific directory.
     */
    std::vector<std::string> listDirectory(const std::string& dirPath) const;

    /**
     * @brief Returns a flat list of all resources in the file system.
     */
    std::vector<std::string> listAllResources() const;

    /**
     * @brief Gets metadata description of a node (type, size, modified).
     */
    std::string getNodeInfo(const std::string& path) const;

    /**
     * @brief Clears the VFS back to empty root.
     */
    void clear();

    /**
     * @brief Standardizes path format (e.g. "/documents/report.txt").
     */
    static std::string formatPath(const std::string& path) {
        return normalizePath(path);
    }
};

#endif // VIRTUAL_FILE_SYSTEM_H
