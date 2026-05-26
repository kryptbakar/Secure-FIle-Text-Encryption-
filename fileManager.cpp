#include "fileManager.h"
#include "encryption.h"
#include <fstream>
#include <iostream>
#include <stdexcept>

#ifdef _WIN32
#include <direct.h>
#define MAKE_DIR(p) _mkdir(p)
#else
#include <sys/stat.h>
#define MAKE_DIR(p) mkdir((p), 0755)
#endif

const char* FileManager::STORAGE_DIR = "secure_storage";
const char* FileManager::INDEX_FILE  = "secure_storage/index.dat";

// ---- Construction / destruction ----

FileManager::FileManager() : head(nullptr) {
    ensureStorageDir();
    loadIndex();
}

FileManager::~FileManager() {
    FileNode* cur = head;
    while (cur) {
        FileNode* nxt = cur->next;
        delete cur;
        cur = nxt;
    }
}

// ---- Private linked-list helpers ----

void FileManager::ensureStorageDir() const {
    MAKE_DIR(STORAGE_DIR); // silently fails if directory already exists
}

void FileManager::appendNode(const std::string& name, const std::string& path) {
    FileNode* node = new FileNode(name, path);
    if (!head) { head = node; return; }
    FileNode* cur = head;
    while (cur->next) cur = cur->next;
    cur->next = node;
}

FileNode* FileManager::findNode(const std::string& name) const {
    for (FileNode* cur = head; cur; cur = cur->next)
        if (cur->name == name) return cur;
    return nullptr;
}

void FileManager::removeNode(const std::string& name) {
    if (!head) return;
    if (head->name == name) {
        FileNode* del = head;
        head = head->next;
        delete del;
        return;
    }
    for (FileNode* cur = head; cur->next; cur = cur->next) {
        if (cur->next->name == name) {
            FileNode* del = cur->next;
            cur->next = del->next;
            delete del;
            return;
        }
    }
}

// ---- Index persistence ----

void FileManager::loadIndex() {
    std::ifstream f(INDEX_FILE);
    if (!f.is_open()) return; // no index yet — fresh start
    std::string line;
    while (std::getline(f, line)) {
        size_t sep = line.find('|');
        if (sep == std::string::npos) continue;
        appendNode(line.substr(0, sep), line.substr(sep + 1));
    }
}

void FileManager::saveIndex() const {
    std::ofstream f(INDEX_FILE);
    if (!f) { std::cerr << "Error: cannot write index file.\n"; return; }
    for (FileNode* cur = head; cur; cur = cur->next)
        f << cur->name << "|" << cur->storagePath << "\n";
}

// ---- Public file operations ----

bool FileManager::createFile(const std::string& name,
                              const std::string& content,
                              const std::string& key) {
    if (name.empty()) {
        std::cerr << "Error: file name cannot be empty.\n";
        return false;
    }
    if (findNode(name)) {
        std::cerr << "Error: file '" << name << "' already exists.\n";
        return false;
    }
    std::string path = std::string(STORAGE_DIR) + "/" + name + ".enc";
    std::ofstream f(path);
    if (!f) {
        std::cerr << "Error: cannot create file on disk.\n";
        return false;
    }
    f << Encryption::encrypt(content, key);
    f.close();
    appendNode(name, path);
    saveIndex();
    std::cout << "File '" << name << "' created and encrypted successfully.\n";
    return true;
}

bool FileManager::deleteFile(const std::string& name) {
    FileNode* node = findNode(name);
    if (!node) {
        std::cerr << "Error: file '" << name << "' not found.\n";
        return false;
    }
    std::remove(node->storagePath.c_str());
    removeNode(name);
    saveIndex();
    std::cout << "File '" << name << "' deleted successfully.\n";
    return true;
}

std::string FileManager::viewFile(const std::string& name, const std::string& key) {
    FileNode* node = findNode(name);
    if (!node) {
        std::cerr << "Error: file '" << name << "' not found.\n";
        return "";
    }
    std::ifstream f(node->storagePath);
    if (!f) {
        std::cerr << "Error: cannot open encrypted file.\n";
        return "";
    }
    std::string hex((std::istreambuf_iterator<char>(f)),
                     std::istreambuf_iterator<char>());
    try {
        return Encryption::decrypt(hex, key);
    } catch (const std::exception& e) {
        std::cerr << "Decryption error: " << e.what() << "\n";
        return "";
    }
}

bool FileManager::searchFile(const std::string& name) const {
    return findNode(name) != nullptr;
}

void FileManager::listFiles() const {
    if (!head) {
        std::cout << "No files stored.\n";
        return;
    }
    std::cout << "\n--- Stored Files ---\n";
    int i = 1;
    for (FileNode* cur = head; cur; cur = cur->next)
        std::cout << "  " << i++ << ". " << cur->name << "\n";
    std::cout << "--------------------\n";
}

int FileManager::fileCount() const {
    int n = 0;
    for (FileNode* cur = head; cur; cur = cur->next) ++n;
    return n;
}

std::vector<std::string> FileManager::getFileNames() const {
    std::vector<std::string> names;
    for (FileNode* cur = head; cur; cur = cur->next)
        names.push_back(cur->name);
    return names;
}
