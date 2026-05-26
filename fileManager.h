#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <string>
#include <vector>

// Singly-linked list node representing one tracked encrypted file.
struct FileNode {
    std::string name;         // Logical file name (key used for lookup)
    std::string storagePath;  // Path to the .enc file on disk
    FileNode*   next;

    FileNode(const std::string& n, const std::string& p)
        : name(n), storagePath(p), next(nullptr) {}
};

// Manages encrypted files using a singly-linked list for in-memory tracking.
// All file content is XOR-encrypted before being written to disk.
// The list is persisted across runs via a plain-text index file.
class FileManager {
public:
    FileManager();
    ~FileManager();

    // Encrypt content with key and store as name.enc; returns false on error.
    bool createFile(const std::string& name,
                    const std::string& content,
                    const std::string& key);

    // Remove file from list and disk; returns false if not found.
    bool deleteFile(const std::string& name);

    // Decrypt and return file content; returns "" on error.
    std::string viewFile(const std::string& name, const std::string& key);

    // Return true if a file with this name exists in the list.
    bool searchFile(const std::string& name) const;

    // Print all file names tracked in the linked list.
    void listFiles() const;

    // Number of files currently tracked.
    int fileCount() const;

    // Return a list of all tracked file names (for GUI display).
    std::vector<std::string> getFileNames() const;

private:
    FileNode* head;

    static const char* STORAGE_DIR;
    static const char* INDEX_FILE;

    void      appendNode(const std::string& name, const std::string& path);
    FileNode* findNode(const std::string& name) const;
    void      removeNode(const std::string& name);
    void      loadIndex();
    void      saveIndex() const;
    void      ensureStorageDir() const;
};

#endif // FILE_MANAGER_H
