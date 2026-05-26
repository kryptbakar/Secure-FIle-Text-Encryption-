#ifndef AUTH_H
#define AUTH_H

#include <string>
#include <unordered_map>

enum class Role { ADMIN, USER, GUEST, NONE };

struct User {
    std::string username;
    std::string passwordHash;
    Role role;
};

// Role-based authentication. Pre-registered accounts:
//   admin / admin123  (ADMIN)
//   user  / user123   (USER)
//   guest / guest123  (GUEST)
class AuthSystem {
public:
    AuthSystem();

    // Sets outRole and returns true on success; false for bad credentials.
    bool login(const std::string& username,
               const std::string& password,
               Role& outRole) const;

    // Registers a new user. Returns false if username already exists.
    bool registerUser(const std::string& username,
                      const std::string& password,
                      Role role);

    static std::string roleName(Role role);

private:
    std::unordered_map<std::string, User> users;
    static std::string hashPassword(const std::string& password);
};

#endif // AUTH_H
