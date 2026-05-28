# Secure File Storage System

[![C++](https://img.shields.io/badge/C%2B%2B-17-blue)](https://en.cppreference.com/w/cpp/17)
[![SFML](https://img.shields.io/badge/SFML-2.6.x-brightgreen)](https://www.sfml-dev.org/)

---

## 🚀 Project Overview
A console + GUI application for securely storing files using **XOR encryption**, **role-based authentication**, and a **linked-list file tracking system**. The project is implemented in **C++17** with an **SFML** graphical interface.

This repository demonstrates:
- implementing a complete mini secure-storage workflow (create/view/delete/search)
- basic cryptography concepts (encryption at rest)
- access control and role separation
- custom data structures (linked list) to manage a file index

---

## 🧾 Recruiter-Friendly Summary
- C++17 secure storage application with GUI (SFML)
- Encrypts data before writing to disk and decrypts on read
- RBAC with ADMIN/USER/GUEST permissions
- Linked list index for file metadata and traversal-based search

---

## ✨ Key Features
- **XOR Encryption** — all files are encrypted before being written to disk
- **Role-Based Access Control** — ADMIN, USER, and GUEST roles with different permissions
- **Linked List File Tracking** — files are managed in-memory using a custom singly-linked list
- **SFML GUI** — login screen, file browser, dialogs
- **Persistent Storage** — encrypted files and index survive across sessions

---

## 🔐 Default Accounts
| Username | Password | Role | Permissions |
|---|---|---|---|
| admin | admin123 | ADMIN | Create, View, Delete, Register users |
| user | user123 | USER | Create, View, Delete |
| guest | guest123 | GUEST | View only |

> Recommendation: if you publish/deploy this project, replace these defaults and store credentials securely.

---

## 🛠️ Tech Stack
- **Language:** C++17
- **GUI:** SFML 2.6.x
- **Build:** g++ / MinGW-w64
- **Platform:** Windows 10+ (current build instructions target Windows)

---

## 📁 Project Structure
```
Secure-File-Storage/
├── main.cpp                          # Entry point — launches the GUI
├── gui.h / gui.cpp                   # SFML graphical interface (login + main screen)
├── auth.h / auth.cpp                 # Authentication and role management
├── fileManager.h / fileManager.cpp   # Linked-list file manager + disk I/O
├── encryption.h / encryption.cpp     # XOR encryption with hex encoding
└── .vscode/
    └── tasks.json                    # VS Code build task
```

---

## 🔎 How It Works (High-Level)
1. **Login** — credentials are verified (djb2-hashed password store)
2. **Create** — plaintext file content is XOR-encrypted and saved to disk (e.g., `.enc`)
3. **View** — encrypted file is loaded, decoded, XOR-decrypted back to plaintext
4. **Delete** — removes file from disk and removes node from linked list
5. **Search** — traverses linked list for file metadata (O(n))

---

## ✅ Requirements
- **Compiler:** GCC (MinGW-w64) with C++17 support
- **Library:** SFML 2.6.x (MinGW 64-bit)
- **OS:** Windows 10 or higher

### Quick setup (Windows)
```powershell
# Install GCC via Scoop (no admin needed)
irm get.scoop.sh | iex
scoop install gcc

# Download SFML 2.6.2 for MinGW 64-bit from https://www.sfml-dev.org/download.php
# Extract to C:\Users\<you>\SFML
```

---

## 🏗️ Build
```bash
g++ -std=c++17 -Wall -o mainsecurity.exe \
    main.cpp encryption.cpp fileManager.cpp auth.cpp gui.cpp \
    -I%USERPROFILE%\SFML\include \
    -L%USERPROFILE%\SFML\lib \
    -DSFML_STATIC \
    -lsfml-graphics-s -lsfml-window-s -lsfml-system-s \
    -lfreetype -lopengl32 -lgdi32 -lwinmm \
    -static-libgcc -static-libstdc++ -mwindows
```

Or press **Ctrl+Shift+B** in VS Code to use the pre-configured build task.

---

## ▶️ Run
```bash
mainsecurity.exe
```

Encrypted files are stored in `secure_storage/` (created automatically on first run).

---

## 🔒 Security Notes (Important)
This project is educational and demonstrates concepts. If you plan to harden it:
- XOR encryption is **not secure** against modern attackers; replace with AES-GCM or libsodium.
- Protect keys properly (do not hardcode; consider OS keychain / DPAPI / KMS).
- Store passwords using a modern hash (Argon2/bcrypt/scrypt) rather than basic hashes.
- Add integrity checks (MAC / AEAD) so attackers can’t tamper with ciphertext.
- Consider replacing linked list indexing with a more efficient structure for scaling.

---

## 🧪 Suggested Test Plan
- Login with each role and verify permissions are enforced
- Create → view → delete cycle with multiple files
- Validate storage persistence across application restarts
- Attempt unauthorized actions as USER/GUEST

---

## 🤝 Contributing
Suggestions and improvements are welcome via issues or pull requests.

---

## 📄 License
Add a `LICENSE` file to clarify redistribution and usage terms.

---

## Appendix: Original README (Preserved)

# Secure File Storage System

A console + GUI application for securely storing files using XOR encryption, role-based authentication, and a linked-list file tracking system. Built in C++17 with an SFML graphical interface.

## Features

- **XOR Encryption** — all files are encrypted before being written to disk
- **Role-Based Access Control** — ADMIN, USER, and GUEST roles with different permissions
- **Linked List File Tracking** — files are managed in-memory using a custom singly-linked list
- **SFML GUI** — dark-themed graphical interface with login screen, file browser, and dialogs
- **Persistent Storage** — encrypted files and index survive across sessions

## Default Accounts

| Username | Password  | Role  | Permissions                        |
|----------|-----------|-------|------------------------------------|
| admin    | admin123  | ADMIN | Create, View, Delete, Register users |
| user     | user123   | USER  | Create, View, Delete               |
| guest    | guest123  | GUEST | View only                          |

## Project Structure

```
Secure-File-Storage/
├── main.cpp            # Entry point — launches the GUI
├── gui.h / gui.cpp     # SFML graphical interface (login + main screen)
├── auth.h / auth.cpp   # Authentication and role management
├── fileManager.h / fileManager.cpp  # Linked-list file manager + disk I/O
├── encryption.h / encryption.cpp    # XOR encryption with hex encoding
└── .vscode/
    └── tasks.json      # VS Code build task
```

## Requirements

- **Compiler:** GCC (MinGW-w64) with C++17 support
- **Library:** [SFML 2.6.x](https://www.sfml-dev.org/) for MinGW 64-bit
- **OS:** Windows 10 or higher

### Quick setup (Windows)

```powershell
# Install GCC via Scoop (no admin needed)
irm get.scoop.sh | iex
scoop install gcc

# Download SFML 2.6.2 for MinGW 64-bit from https://www.sfml-dev.org/download.php
# Extract to C:\Users\<you>\SFML
```

## Build

```bash
g++ -std=c++17 -Wall -o mainsecurity.exe \
    main.cpp encryption.cpp fileManager.cpp auth.cpp gui.cpp \
    -I%USERPROFILE%\SFML\include \
    -L%USERPROFILE%\SFML\lib \
    -DSFML_STATIC \
    -lsfml-graphics-s -lsfml-window-s -lsfml-system-s \
    -lfreetype -lopengl32 -lgdi32 -lwinmm \
    -static-libgcc -static-libstdc++ -mwindows
```

Or press **Ctrl+Shift+B** in VS Code to use the pre-configured build task.

## Run

```bash
mainsecurity.exe
```

Encrypted files are stored in `secure_storage/` (created automatically on first run).

## How It Works

1. **Login** — credentials are verified using a djb2-hashed password store
2. **Create** — file content is XOR-encrypted with a session key and saved as `.enc`
3. **View** — `.enc` file is read, hex-decoded, and XOR-decrypted back to plaintext
4. **Delete** — removes the `.enc` file from disk and the node from the linked list
5. **Search** — traverses the linked list for a matching file name (O(n))
