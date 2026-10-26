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

class VFSFile : public VFSNode {
public:
    std::string content;

    VFSFile(const std::string& n, const std::string& path, const std::string& initialContent = "");
    void write(const std::string& newContent);
    size_t getSize() const;
};

class VFSDirectory : public VFSNode {
public:
    // Sorted map for deterministic order
    std::map<std::string, std::shared_ptr<VFSNode>> children;

    VFSDirectory(const std::string& n, const std::string& path);
};


 //Manages an in-memory hierarchical file system with files and directories.
 
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

    bool createDirectory(const std::string& path);
    bool createFile(const std::string& path, const std::string& initialContent = "");
    bool exists(const std::string& path) const;
    bool isFile(const std::string& path) const;
    bool isDirectory(const std::string& path) const;
    bool readFile(const std::string& path, std::string& outContent) const;
    bool writeFile(const std::string& path, const std::string& content);
    bool deleteResource(const std::string& path);
    std::vector<std::string> listDirectory(const std::string& dirPath) const;
    std::vector<std::string> listAllResources() const;
    std::string getNodeInfo(const std::string& path) const;
    void clear();

    static std::string formatPath(const std::string& path) {
        return normalizePath(path);
    }
};

#endif 
