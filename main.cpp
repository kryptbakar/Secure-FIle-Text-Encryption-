#include <iostream>
#include <string>
#include <limits>
#include "auth.h"
#include "fileManager.h"
#include "encryption.h"
#include "gui.h"

int main() {
    AuthSystem  auth;
    FileManager fileManager;
    const std::string ENC_KEY = "SecureKey@2024!!";

    GUI gui(auth, fileManager, ENC_KEY);
    gui.run();

    return 0;
}
