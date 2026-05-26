#include "auth.h"
#include <iostream>

std::string AuthSystem::hashPassword(const std::string& pw) {
    // djb2 hash — not cryptographic, sufficient for this educational project.
    unsigned long h = 5381;
    for (unsigned char c : pw)
        h = ((h << 5) + h) + c;
    return std::to_string(h);
}

AuthSystem::AuthSystem() {
    registerUser("admin", "admin123", Role::ADMIN);
    registerUser("user",  "user123",  Role::USER);
    registerUser("guest", "guest123", Role::GUEST);
}

bool AuthSystem::login(const std::string& username,
                       const std::string& password,
                       Role& outRole) const {
    auto it = users.find(username);
    if (it == users.end()) return false;
    if (it->second.passwordHash != hashPassword(password)) return false;
    outRole = it->second.role;
    return true;
}

bool AuthSystem::registerUser(const std::string& username,
                               const std::string& password,
                               Role role) {
    if (users.count(username)) {
        std::cerr << "User '" << username << "' already exists.\n";
        return false;
    }
    users[username] = {username, hashPassword(password), role};
    return true;
}

std::string AuthSystem::roleName(Role role) {
    switch (role) {
        case Role::ADMIN: return "ADMIN";
        case Role::USER:  return "USER";
        case Role::GUEST: return "GUEST";
        default:          return "NONE";
    }
}
