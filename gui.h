#ifndef GUI_H
#define GUI_H

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include "auth.h"
#include "fileManager.h"

// ---- Reusable text-input widget ----
struct InputBox {
    sf::RectangleShape box;
    sf::Text           label;
    std::string        buffer;
    bool               focused = false;
    bool               masked  = false;   // show '*' instead of real chars

    void init(const sf::Font& font, float x, float y,
              float w, float h, unsigned int charSize = 17);
    void handleEvent(const sf::Event& e);
    void draw(sf::RenderWindow& w);
    void clear() { buffer.clear(); }
};

// ---- Screen / dialog states ----
enum class Screen   { LOGIN, MAIN };
enum class Dialog   { NONE, CREATE, REGISTER };

// ---- Main GUI class ----
class GUI {
public:
    GUI(AuthSystem& auth, FileManager& fm, const std::string& encKey);
    void run();

private:
    AuthSystem&       auth;
    FileManager&      fm;
    const std::string ENC_KEY;

    sf::RenderWindow  window;
    sf::Font          font;

    Screen  screen  = Screen::LOGIN;
    Dialog  dialog  = Dialog::NONE;
    Role    role    = Role::NONE;

    std::string loggedUser;
    std::string statusMsg;
    std::string viewContent;
    std::string viewingFile;
    int         selectedIdx = -1;
    bool        statusIsError = false;

    // Login widgets
    InputBox loginUser;
    InputBox loginPass;

    // Create-file dialog widgets
    InputBox createName;
    InputBox createContent;

    // Register-user dialog widgets
    InputBox regUser;
    InputBox regPass;
    InputBox regRole;

    // ---- helpers ----
    void setStatus(const std::string& msg, bool isError = false);

    sf::Text makeText(const std::string& str, unsigned sz,
                      sf::Color col, float x, float y);
    sf::RectangleShape makeRect(float x, float y, float w, float h,
                                sf::Color fill, sf::Color outline = sf::Color::Transparent,
                                float outlineThick = 0.f);
    bool clicked(const sf::RectangleShape& r, const sf::Event& e);
    bool clicked(sf::FloatRect r, const sf::Event& e);

    // ---- screens ----
    void runLogin();
    void runMain();

    // ---- login helpers ----
    void tryLogin();

    // ---- main screen sections ----
    void drawHeader();
    void drawFilePanel(const std::vector<std::string>& files,
                       const sf::Event& e);
    void drawContentPanel();
    void drawButtonBar(const sf::Event& e,
                       const std::vector<std::string>& files);
    void drawStatusBar();

    // ---- dialogs ----
    void drawCreateDialog(const sf::Event& e);
    void drawRegisterDialog(const sf::Event& e);
};

#endif // GUI_H
