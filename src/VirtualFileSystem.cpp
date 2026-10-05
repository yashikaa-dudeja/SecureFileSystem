#include "VirtualFileSystem.h"
#include "Models.h"
#include <sstream>
#include <algorithm>

VFSNode::VFSNode(const std::string& n, const std::string& path, NodeType t)
    : name(n), fullPath(path), type(t),
      createdAt(getCurrentTimestamp()), modifiedAt(getCurrentTimestamp()) {}

VFSFile::VFSFile(const std::string& n, const std::string& path, const std::string& initialContent)
    : VFSNode(n, path, NodeType::FILE_NODE), content(initialContent) {}

void VFSFile::write(const std::string& newContent) {
    content = newContent;
    modifiedAt = getCurrentTimestamp();
}

size_t VFSFile::getSize() const {
    return content.size();
}

VFSDirectory::VFSDirectory(const std::string& n, const std::string& path)
    : VFSNode(n, path, NodeType::DIRECTORY_NODE) {}

VirtualFileSystem::VirtualFileSystem() {
    clear();
}

void VirtualFileSystem::clear() {
    root = std::make_shared<VFSDirectory>("", "/");
}

std::string VirtualFileSystem::normalizePath(const std::string& rawPath) {
    if (rawPath.empty()) return "/";

    std::string s = rawPath;
    // Replace backslashes with forward slashes
    std::replace(s.begin(), s.end(), '\\', '/');

    // Ensure leading slash
    if (s.front() != '/') {
        s = "/" + s;
    }

    // Collapse multiple consecutive slashes
    std::string normalized;
    bool lastWasSlash = false;
    for (char c : s) {
        if (c == '/') {
            if (!lastWasSlash) {
                normalized.push_back(c);
                lastWasSlash = true;
            }
        } else {
            normalized.push_back(c);
            lastWasSlash = false;
        }
    }

    // Remove trailing slash if path is longer than 1 character
    if (normalized.length() > 1 && normalized.back() == '/') {
        normalized.pop_back();
    }

    return normalized;
}

std::vector<std::string> VirtualFileSystem::splitPath(const std::string& normalizedPath) {
    std::vector<std::string> parts;
    std::stringstream ss(normalizedPath);
    std::string segment;
    while (std::getline(ss, segment, '/')) {
        if (!segment.empty()) {
            parts.push_back(segment);
        }
    }
    return parts;
}

std::shared_ptr<VFSNode> VirtualFileSystem::findNode(const std::string& normalizedPath) const {
    if (normalizedPath == "/" || normalizedPath.empty()) {
        return root;
    }

    auto parts = splitPath(normalizedPath);
    std::shared_ptr<VFSNode> curr = root;

    for (const auto& part : parts) {
        if (curr->type != NodeType::DIRECTORY_NODE) {
            return nullptr; // A file cannot have children
        }
        auto dir = std::static_pointer_cast<VFSDirectory>(curr);
        auto it = dir->children.find(part);
        if (it == dir->children.end()) {
            return nullptr;
        }
        curr = it->second;
    }

    return curr;
}

std::shared_ptr<VFSDirectory> VirtualFileSystem::findParentDirectory(
    const std::string& normalizedPath, std::string& outBaseName) const {
    if (normalizedPath == "/" || normalizedPath.empty()) {
        outBaseName = "";
        return nullptr; // Root has no parent directory
    }

    auto parts = splitPath(normalizedPath);
    outBaseName = parts.back();
    parts.pop_back();

    std::shared_ptr<VFSNode> curr = root;
    for (const auto& part : parts) {
        if (curr->type != NodeType::DIRECTORY_NODE) {
            return nullptr;
        }
        auto dir = std::static_pointer_cast<VFSDirectory>(curr);
        auto it = dir->children.find(part);
        if (it == dir->children.end()) {
            return nullptr;
        }
        curr = it->second;
    }

    if (curr->type == NodeType::DIRECTORY_NODE) {
        return std::static_pointer_cast<VFSDirectory>(curr);
    }
    return nullptr;
}

bool VirtualFileSystem::createDirectory(const std::string& rawPath) {
    std::string path = normalizePath(rawPath);
    if (path == "/") return true; // Root already exists

    auto parts = splitPath(path);
    std::shared_ptr<VFSDirectory> curr = root;
    std::string currentPath = "";

    for (const auto& part : parts) {
        currentPath += "/" + part;
        auto it = curr->children.find(part);
        if (it == curr->children.end()) {
            auto newDir = std::make_shared<VFSDirectory>(part, currentPath);
            curr->children[part] = newDir;
            curr = newDir;
        } else {
            if (it->second->type != NodeType::DIRECTORY_NODE) {
                return false; // A file exists with this intermediate path name
            }
            curr = std::static_pointer_cast<VFSDirectory>(it->second);
        }
    }
    return true;
}

bool VirtualFileSystem::createFile(const std::string& rawPath, const std::string& initialContent) {
    std::string path = normalizePath(rawPath);
    if (path == "/") return false; // Cannot create a file named root

    // If node already exists, do not recreate
    if (exists(path)) {
        return false;
    }

    std::string fileName;
    auto parentDir = findParentDirectory(path, fileName);
    if (!parentDir) {
        // Automatically create parent directory path if not already there
        size_t lastSlash = path.find_last_of('/');
        std::string parentPath = (lastSlash == 0) ? "/" : path.substr(0, lastSlash);
        if (!createDirectory(parentPath)) {
            return false;
        }
        parentDir = findParentDirectory(path, fileName);
        if (!parentDir) return false;
    }

    auto newFile = std::make_shared<VFSFile>(fileName, path, initialContent);
    parentDir->children[fileName] = newFile;
    return true;
}

bool VirtualFileSystem::exists(const std::string& rawPath) const {
    return findNode(normalizePath(rawPath)) != nullptr;
}

bool VirtualFileSystem::isFile(const std::string& rawPath) const {
    auto node = findNode(normalizePath(rawPath));
    return node && (node->type == NodeType::FILE_NODE);
}

bool VirtualFileSystem::isDirectory(const std::string& rawPath) const {
    auto node = findNode(normalizePath(rawPath));
    return node && (node->type == NodeType::DIRECTORY_NODE);
}

bool VirtualFileSystem::readFile(const std::string& rawPath, std::string& outContent) const {
    auto node = findNode(normalizePath(rawPath));
    if (!node || node->type != NodeType::FILE_NODE) {
        return false;
    }
    auto file = std::static_pointer_cast<VFSFile>(node);
    outContent = file->content;
    return true;
}

bool VirtualFileSystem::writeFile(const std::string& rawPath, const std::string& content) {
    auto node = findNode(normalizePath(rawPath));
    if (!node || node->type != NodeType::FILE_NODE) {
        return false;
    }
    auto file = std::static_pointer_cast<VFSFile>(node);
    file->write(content);
    return true;
}

bool VirtualFileSystem::deleteResource(const std::string& rawPath) {
    std::string path = normalizePath(rawPath);
    if (path == "/") return false; // Cannot delete root directory

    std::string baseName;
    auto parentDir = findParentDirectory(path, baseName);
    if (!parentDir) return false;

    auto it = parentDir->children.find(baseName);
    if (it == parentDir->children.end()) {
        return false;
    }

    parentDir->children.erase(it);
    return true;
}

std::vector<std::string> VirtualFileSystem::listDirectory(const std::string& rawPath) const {
    std::vector<std::string> results;
    std::string path = normalizePath(rawPath);
    auto node = findNode(path);
    if (!node || node->type != NodeType::DIRECTORY_NODE) {
        return results;
    }

    auto dir = std::static_pointer_cast<VFSDirectory>(node);
    for (const auto& pair : dir->children) {
        std::string suffix = (pair.second->type == NodeType::DIRECTORY_NODE) ? "/" : "";
        results.push_back(pair.second->fullPath + suffix);
    }
    return results;
}

void VirtualFileSystem::collectAllResources(
    const std::shared_ptr<VFSDirectory>& dir, std::vector<std::string>& outList) const {
    if (!dir) return;

    for (const auto& pair : dir->children) {
        if (pair.second->type == NodeType::DIRECTORY_NODE) {
            outList.push_back(pair.second->fullPath + "/");
            collectAllResources(std::static_pointer_cast<VFSDirectory>(pair.second), outList);
        } else {
            outList.push_back(pair.second->fullPath);
        }
    }
}

std::vector<std::string> VirtualFileSystem::listAllResources() const {
    std::vector<std::string> results;
    collectAllResources(root, results);
    return results;
}

std::string VirtualFileSystem::getNodeInfo(const std::string& rawPath) const {
    std::string path = normalizePath(rawPath);
    auto node = findNode(path);
    if (!node) return "Resource does not exist.";

    std::ostringstream oss;
    oss << "Path: " << node->fullPath << "\n";
    oss << "Type: " << (node->type == NodeType::FILE_NODE ? "File" : "Directory") << "\n";
    oss << "Created: " << node->createdAt << "\n";
    oss << "Modified: " << node->modifiedAt << "\n";
    if (node->type == NodeType::FILE_NODE) {
        auto f = std::static_pointer_cast<VFSFile>(node);
        oss << "Size: " << f->getSize() << " bytes\n";
    } else {
        auto d = std::static_pointer_cast<VFSDirectory>(node);
        oss << "Items: " << d->children.size() << "\n";
    }
    return oss.str();
}
