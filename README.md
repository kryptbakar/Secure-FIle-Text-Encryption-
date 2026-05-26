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
