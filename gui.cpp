#include "gui.h"
#include <iostream>
#include <sstream>

// ============================================================
//  Colours / layout constants
// ============================================================
static const sf::Color C_BG        {28,  28,  42};
static const sf::Color C_PANEL     {40,  40,  58};
static const sf::Color C_HEADER    {20,  20,  34};
static const sf::Color C_ACCENT    {88,  130, 215};
static const sf::Color C_ACCENT2   {50,  180, 120};
static const sf::Color C_RED       {210, 70,  70};
static const sf::Color C_TEXT      {220, 220, 235};
static const sf::Color C_SUBTEXT   {140, 140, 165};
static const sf::Color C_WHITE     {255, 255, 255};
static const sf::Color C_INPUT_BG  {55,  55,  75};
static const sf::Color C_SEL       {60,  90,  155};

static const float WIN_W  = 920.f;
static const float WIN_H  = 600.f;
static const float HDR_H  = 52.f;
static const float BAR_H  = 42.f;      // status bar
static const float BTN_H  = 40.f;
static const float FILE_W = 220.f;

// ============================================================
//  InputBox
// ============================================================
void InputBox::init(const sf::Font& font, float x, float y,
                    float w, float h, unsigned int cs) {
    box.setPosition(x, y);
    box.setSize({w, h});
    box.setFillColor(C_INPUT_BG);
    box.setOutlineThickness(1.5f);
    box.setOutlineColor({100, 100, 130});

    label.setFont(font);
    label.setCharacterSize(cs);
    label.setFillColor(C_TEXT);
    label.setPosition(x + 8.f, y + (h - cs) / 2.f - 1.f);
}

void InputBox::handleEvent(const sf::Event& e) {
    if (!focused) return;
    if (e.type == sf::Event::TextEntered) {
        uint32_t c = e.text.unicode;
        if ((c == 8 || c == 127) && !buffer.empty())
            buffer.pop_back();
        else if (c >= 32 && c < 127)
            buffer += static_cast<char>(c);
    }
}

void InputBox::draw(sf::RenderWindow& w) {
    box.setOutlineColor(focused ? C_ACCENT : sf::Color{100, 100, 130});
    w.draw(box);
    std::string shown = masked ? std::string(buffer.size(), '*') : buffer;
    label.setString(shown + (focused ? "|" : ""));
    w.draw(label);
}

// ============================================================
//  GUI constructor
// ============================================================
GUI::GUI(AuthSystem& auth, FileManager& fm, const std::string& encKey)
    : auth(auth), fm(fm), ENC_KEY(encKey),
      window(sf::VideoMode((unsigned)WIN_W, (unsigned)WIN_H),
             "Secure File Storage System",
             sf::Style::Titlebar | sf::Style::Close)
{
    window.setFramerateLimit(60);

    // Try system fonts
    bool ok = font.loadFromFile("C:\\Windows\\Fonts\\segoeui.ttf")  ||
              font.loadFromFile("C:\\Windows\\Fonts\\arial.ttf")    ||
              font.loadFromFile("C:\\Windows\\Fonts\\calibri.ttf")  ||
              font.loadFromFile("C:\\Windows\\Fonts\\tahoma.ttf");
    if (!ok) {
        std::cerr << "[GUI] Could not load any system font.\n";
        window.close();
        return;
    }

    // ---- Login widgets ----
    loginUser.init(font, 290, 250, 280, 34);
    loginPass.init(font, 290, 310, 280, 34);
    loginPass.masked  = true;
    loginUser.focused = true;

    // ---- Create-file dialog ----
    createName.init(font,    270, 230, 360, 32);
    createContent.init(font, 270, 295, 360, 32);

    // ---- Register-user dialog ----
    regUser.init(font, 270, 215, 340, 32);
    regPass.init(font, 270, 275, 340, 32);
    regPass.masked = true;
    regRole.init(font, 270, 335, 340, 32);
    regUser.focused = true;
}

// ============================================================
//  Helpers
// ============================================================
void GUI::setStatus(const std::string& msg, bool isError) {
    statusMsg     = msg;
    statusIsError = isError;
}

sf::Text GUI::makeText(const std::string& str, unsigned sz,
                       sf::Color col, float x, float y) {
    sf::Text t;
    t.setFont(font);
    t.setString(str);
    t.setCharacterSize(sz);
    t.setFillColor(col);
    t.setPosition(x, y);
    return t;
}

sf::RectangleShape GUI::makeRect(float x, float y, float w, float h,
                                  sf::Color fill, sf::Color outline, float thick) {
    sf::RectangleShape r({w, h});
    r.setPosition(x, y);
    r.setFillColor(fill);
    if (thick > 0.f) { r.setOutlineColor(outline); r.setOutlineThickness(thick); }
    return r;
}

bool GUI::clicked(const sf::RectangleShape& r, const sf::Event& e) {
    return e.type == sf::Event::MouseButtonPressed &&
           e.mouseButton.button == sf::Mouse::Left &&
           r.getGlobalBounds().contains((float)e.mouseButton.x, (float)e.mouseButton.y);
}
bool GUI::clicked(sf::FloatRect r, const sf::Event& e) {
    return e.type == sf::Event::MouseButtonPressed &&
           e.mouseButton.button == sf::Mouse::Left &&
           r.contains((float)e.mouseButton.x, (float)e.mouseButton.y);
}

// ============================================================
//  run() dispatcher
// ============================================================
void GUI::run() {
    if (!window.isOpen()) return;
    while (window.isOpen()) {
        if (screen == Screen::LOGIN) runLogin();
        else                          runMain();
    }
}

// ============================================================
//  LOGIN SCREEN
// ============================================================
void GUI::tryLogin() {
    Role r = Role::NONE;
    if (auth.login(loginUser.buffer, loginPass.buffer, r)) {
        loggedUser = loginUser.buffer;
        role       = r;
        loginUser.clear(); loginPass.clear();
        setStatus("");
        screen = Screen::MAIN;
    } else {
        loginPass.clear();
        setStatus("Invalid username or password.", true);
    }
}

void GUI::runLogin() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) { window.close(); return; }

        // Tab switches focus between fields
        if (event.type == sf::Event::KeyPressed &&
            event.key.code == sf::Keyboard::Tab) {
            loginUser.focused = !loginUser.focused;
            loginPass.focused = !loginPass.focused;
        }
        if (event.type == sf::Event::KeyPressed &&
            event.key.code == sf::Keyboard::Return)
            tryLogin();

        // Click to focus / click login button
        if (event.type == sf::Event::MouseButtonPressed) {
            loginUser.focused = loginUser.box.getGlobalBounds().contains(
                (float)event.mouseButton.x, (float)event.mouseButton.y);
            loginPass.focused = !loginUser.focused &&
                loginPass.box.getGlobalBounds().contains(
                    (float)event.mouseButton.x, (float)event.mouseButton.y);

            sf::FloatRect btn{340.f, 368.f, 200.f, 42.f};
            if (btn.contains((float)event.mouseButton.x, (float)event.mouseButton.y))
                tryLogin();
        }

        loginUser.handleEvent(event);
        loginPass.handleEvent(event);
    }

    // ---- Draw ----
    window.clear(C_BG);

    // Decorative top bar
    window.draw(makeRect(0, 0, WIN_W, 6, C_ACCENT));

    // Card
    window.draw(makeRect(180, 130, 540, 350, C_PANEL, C_ACCENT, 1.5f));

    // Title
    window.draw(makeText("SECURE FILE STORAGE",  26, C_WHITE,  240, 155));
    window.draw(makeText("SYSTEM",               26, C_ACCENT, 430, 155));
    window.draw(makeText("Sign in to your account", 14, C_SUBTEXT, 323, 195));

    // Divider
    window.draw(makeRect(200, 224, 500, 1, sf::Color{60, 60, 80}));

    // Field labels
    window.draw(makeText("Username", 13, C_SUBTEXT, 290, 232));
    window.draw(makeText("Password", 13, C_SUBTEXT, 290, 292));

    loginUser.box.setPosition(290, 248);
    loginPass.box.setPosition(290, 308);
    loginUser.draw(window);
    loginPass.draw(window);

    // Login button
    auto btn = makeRect(340, 368, 200, 42, C_ACCENT);
    window.draw(btn);
    window.draw(makeText("LOGIN", 17, C_WHITE, 406, 378));

    // Hint
    window.draw(makeText("admin/admin123  \xe2\x80\xa2  user/user123  \xe2\x80\xa2  guest/guest123",
                          12, C_SUBTEXT, 245, 428));

    // Error
    if (!statusMsg.empty())
        window.draw(makeText(statusMsg, 14, C_RED, 263, 462));

    window.display();
}

// ============================================================
//  MAIN SCREEN
// ============================================================
void GUI::runMain() {
    std::vector<std::string> files = fm.getFileNames();

    sf::Event ev;
    std::vector<sf::Event> evts;
    while (window.pollEvent(ev)) {
        if (ev.type == sf::Event::Closed) { window.close(); return; }
        evts.push_back(ev);
    }

    sf::Event noEv{};

    // Route events when no dialog is open
    if (dialog == Dialog::NONE) {
        for (auto& e : evts) {
            float listTop = HDR_H + 40.f;
            for (int i = 0; i < (int)files.size(); ++i) {
                if (clicked({0.f, listTop + i * 32.f, FILE_W, 32.f}, e)) {
                    selectedIdx = i;
                    viewingFile = files[i];
                    viewContent = fm.viewFile(viewingFile, ENC_KEY);
                    setStatus("Viewing '" + viewingFile + "'.");
                }
            }
            drawButtonBar(e, files);
        }
    }

    // ---- Draw ----
    window.clear(C_BG);
    drawHeader();
    drawFilePanel(files, noEv);
    drawContentPanel();
    if (dialog == Dialog::NONE) drawButtonBar(noEv, files);
    drawStatusBar();

    if (dialog == Dialog::CREATE) {
        for (auto& e : evts) drawCreateDialog(e);
        if (evts.empty()) drawCreateDialog(noEv);
    }
    if (dialog == Dialog::REGISTER) {
        for (auto& e : evts) drawRegisterDialog(e);
        if (evts.empty()) drawRegisterDialog(noEv);
    }

    window.display();
}

// ---- Header ----
void GUI::drawHeader() {
    window.draw(makeRect(0, 0, WIN_W, HDR_H, C_HEADER));
    window.draw(makeRect(0, HDR_H - 2, WIN_W, 2, C_ACCENT));

    window.draw(makeText("SECURE FILE STORAGE SYSTEM", 18, C_WHITE,  18, 14));
    std::string info = loggedUser + "  [" + AuthSystem::roleName(role) + "]";
    window.draw(makeText(info, 14, C_SUBTEXT, WIN_W - 280, 18));

    // Logout button (top-right) — click is handled in drawButtonBar
    auto logoutBtn = makeRect(WIN_W - 90, 10, 80, 32, C_RED);
    window.draw(logoutBtn);
    window.draw(makeText("Logout", 14, C_WHITE, WIN_W - 75, 18));
}

// ---- File panel (left) ----
void GUI::drawFilePanel(const std::vector<std::string>& files, const sf::Event&) {
    float panelH = WIN_H - HDR_H - BAR_H - BTN_H - 10;
    window.draw(makeRect(0, HDR_H, FILE_W, panelH, C_PANEL));
    window.draw(makeRect(FILE_W, HDR_H, 2, panelH, sf::Color{60,60,80}));

    std::string hdr = "Files (" + std::to_string(files.size()) + ")";
    window.draw(makeText(hdr, 13, C_SUBTEXT, 10, HDR_H + 10));
    window.draw(makeRect(0, HDR_H + 32, FILE_W, 1, sf::Color{60,60,80}));

    float y = HDR_H + 40.f;
    for (int i = 0; i < (int)files.size(); ++i) {
        sf::Color bg = (i == selectedIdx) ? C_SEL : sf::Color::Transparent;
        window.draw(makeRect(0, y, FILE_W, 30, bg));
        sf::Color fc = (i == selectedIdx) ? C_WHITE : C_TEXT;

        // Truncate long names
        std::string name = files[i];
        if (name.size() > 22) name = name.substr(0, 20) + "..";
        window.draw(makeText((i == selectedIdx ? "> " : "  ") + name, 14, fc, 8, y + 6));
        y += 32.f;
    }
}

// ---- Content panel (right) ----
void GUI::drawContentPanel() {
    float px = FILE_W + 2.f;
    float py = HDR_H;
    float pw = WIN_W - px;
    float ph = WIN_H - HDR_H - BAR_H - BTN_H - 10;

    window.draw(makeRect(px, py, pw, ph, C_BG));

    if (viewingFile.empty()) {
        window.draw(makeText("Select a file from the list to view its content.",
                             14, C_SUBTEXT, px + 30, py + ph / 2 - 10));
        return;
    }

    window.draw(makeText("File: " + viewingFile, 14, C_ACCENT, px + 12, py + 10));
    window.draw(makeRect(px + 8, py + 32, pw - 16, 1, sf::Color{60,60,80}));

    // Word-wrap content display
    float ty = py + 42.f;
    std::istringstream ss(viewContent);
    std::string word, line;
    float maxW = pw - 30.f;
    sf::Text probe;
    probe.setFont(font);
    probe.setCharacterSize(14);

    auto flush = [&]() {
        if (!line.empty()) {
            window.draw(makeText(line, 14, C_TEXT, px + 12, ty));
            ty += 20.f;
            line.clear();
        }
    };

    for (char c : viewContent) {
        if (c == '\n') { flush(); continue; }
        line += c;
        probe.setString(line);
        if (probe.getLocalBounds().width > maxW) {
            size_t sp = line.rfind(' ');
            if (sp != std::string::npos) {
                std::string rest = line.substr(sp + 1);
                line = line.substr(0, sp);
                flush();
                line = rest;
            } else flush();
        }
        if (ty > py + ph - 25.f) break;
    }
    flush();
}

// ---- Button bar ----
void GUI::drawButtonBar(const sf::Event& e, const std::vector<std::string>& files) {
    float by = WIN_H - BAR_H - BTN_H - 4.f;
    window.draw(makeRect(0, by - 2, WIN_W, 2, sf::Color{60,60,80}));
    window.draw(makeRect(0, by, WIN_W, BTN_H + 4, C_HEADER));

    float bx = 10.f, bw = 110.f, bh = 34.f, gap = 12.f;
    float fy = by + 4.f;

    // Create
    auto bCreate = makeRect(bx, fy, bw, bh, C_ACCENT2);
    window.draw(bCreate);
    window.draw(makeText("+ Create", 14, C_WHITE, bx + 18, fy + 8));
    if (clicked(bCreate, e) && role != Role::GUEST) {
        dialog = Dialog::CREATE;
        createName.clear(); createContent.clear(); createName.focused = true;
    } else if (clicked(bCreate, e) && role == Role::GUEST)
        setStatus("GUEST cannot create files.", true);
    bx += bw + gap;

    // View
    auto bView = makeRect(bx, fy, bw, bh, C_ACCENT);
    window.draw(bView);
    window.draw(makeText("View", 14, C_WHITE, bx + 32, fy + 8));
    if (clicked(bView, e)) {
        if (selectedIdx >= 0 && selectedIdx < (int)files.size()) {
            viewingFile = files[selectedIdx];
            viewContent = fm.viewFile(viewingFile, ENC_KEY);
            setStatus("Viewing '" + viewingFile + "'.");
        } else setStatus("Select a file first.", true);
    }
    bx += bw + gap;

    // Delete
    if (role != Role::GUEST) {
        auto bDel = makeRect(bx, fy, bw, bh, C_RED);
        window.draw(bDel);
        window.draw(makeText("Delete", 14, C_WHITE, bx + 28, fy + 8));
        if (clicked(bDel, e)) {
            if (selectedIdx >= 0 && selectedIdx < (int)files.size()) {
                std::string name = files[selectedIdx];
                fm.deleteFile(name);
                if (viewingFile == name) { viewingFile.clear(); viewContent.clear(); }
                selectedIdx = -1;
                setStatus("Deleted '" + name + "'.");
            } else setStatus("Select a file first.", true);
        }
        bx += bw + gap;
    }

    // Register user (ADMIN only)
    if (role == Role::ADMIN) {
        auto bReg = makeRect(bx, fy, 130, bh, sf::Color{90, 60, 150});
        window.draw(bReg);
        window.draw(makeText("Add User", 14, C_WHITE, bx + 20, fy + 8));
        if (clicked(bReg, e)) {
            dialog = Dialog::REGISTER;
            regUser.clear(); regPass.clear(); regRole.clear();
            regUser.focused = true;
        }
        bx += 130.f + gap;
    }

    // Logout (far right matches header button area)
    auto bLogout = makeRect(WIN_W - 95, fy, 85, bh, C_RED);
    window.draw(bLogout);
    window.draw(makeText("Logout", 14, C_WHITE, WIN_W - 78, fy + 8));
    if (clicked(bLogout, e)) {
        screen      = Screen::LOGIN;
        loggedUser.clear(); viewContent.clear(); viewingFile.clear();
        selectedIdx = -1; dialog = Dialog::NONE;
        loginUser.focused = true;
        setStatus("");
    }
}

// ---- Status bar ----
void GUI::drawStatusBar() {
    float sy = WIN_H - BAR_H;
    window.draw(makeRect(0, sy, WIN_W, BAR_H, C_HEADER));
    window.draw(makeRect(0, sy, WIN_W, 1, sf::Color{60,60,80}));
    sf::Color sc = statusIsError ? C_RED : C_ACCENT2;
    window.draw(makeText(statusMsg.empty() ? "Ready." : statusMsg, 13, sc, 14, sy + 12));
}

// ============================================================
//  CREATE FILE DIALOG
// ============================================================
void GUI::drawCreateDialog(const sf::Event& e) {
    // Darken background
    sf::RectangleShape overlay({WIN_W, WIN_H});
    overlay.setFillColor({0, 0, 0, 160});
    window.draw(overlay);

    float dx = 200, dy = 170, dw = 500, dh = 240;
    window.draw(makeRect(dx, dy, dw, dh, C_PANEL, C_ACCENT, 2.f));
    window.draw(makeRect(dx, dy, dw, 40, C_HEADER));
    window.draw(makeText("Create New File", 17, C_WHITE, dx + 16, dy + 10));

    window.draw(makeText("File name:", 13, C_SUBTEXT, dx + 20, dy + 55));
    window.draw(makeText("Content:",   13, C_SUBTEXT, dx + 20, dy + 120));

    createName.box.setPosition(dx + 20, dy + 72);
    createName.box.setSize({dw - 40, 32});
    createContent.box.setPosition(dx + 20, dy + 137);
    createContent.box.setSize({dw - 40, 32});
    createName.draw(window);
    createContent.draw(window);

    // Buttons
    auto bOk  = makeRect(dx + dw - 200, dy + dh - 48, 85, 32, C_ACCENT2);
    auto bCan = makeRect(dx + dw - 105, dy + dh - 48, 85, 32, sf::Color{90,60,60});
    window.draw(bOk);  window.draw(makeText("Create", 14, C_WHITE, dx + dw - 185, dy + dh - 40));
    window.draw(bCan); window.draw(makeText("Cancel", 14, C_WHITE, dx + dw - 90,  dy + dh - 40));

    // Tab between fields
    if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::Tab) {
        createName.focused    = !createName.focused;
        createContent.focused = !createContent.focused;
    }
    // Click focus
    if (e.type == sf::Event::MouseButtonPressed) {
        createName.focused    = createName.box.getGlobalBounds().contains(
            (float)e.mouseButton.x, (float)e.mouseButton.y);
        createContent.focused = !createName.focused &&
            createContent.box.getGlobalBounds().contains(
                (float)e.mouseButton.x, (float)e.mouseButton.y);
    }

    createName.handleEvent(e);
    createContent.handleEvent(e);

    if (clicked(bOk, e) ||
        (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::Return)) {
        if (!createName.buffer.empty()) {
            bool ok = fm.createFile(createName.buffer, createContent.buffer, ENC_KEY);
            setStatus(ok ? "File '" + createName.buffer + "' created." :
                          "Error: file already exists.", !ok);
        } else setStatus("File name cannot be empty.", true);
        dialog = Dialog::NONE;
    }
    if (clicked(bCan, e) ||
        (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::Escape)) {
        dialog = Dialog::NONE;
        setStatus("Create cancelled.");
    }
}

// ============================================================
//  REGISTER USER DIALOG  (ADMIN only)
// ============================================================
void GUI::drawRegisterDialog(const sf::Event& e) {
    sf::RectangleShape overlay({WIN_W, WIN_H});
    overlay.setFillColor({0, 0, 0, 160});
    window.draw(overlay);

    float dx = 190, dy = 145, dw = 520, dh = 280;
    window.draw(makeRect(dx, dy, dw, dh, C_PANEL, sf::Color{90,60,150}, 2.f));
    window.draw(makeRect(dx, dy, dw, 40, C_HEADER));
    window.draw(makeText("Register New User", 17, C_WHITE, dx + 16, dy + 10));

    window.draw(makeText("Username:", 13, C_SUBTEXT, dx + 20, dy + 55));
    window.draw(makeText("Password:", 13, C_SUBTEXT, dx + 20, dy + 115));
    window.draw(makeText("Role (ADMIN / USER / GUEST):", 13, C_SUBTEXT, dx + 20, dy + 175));

    regUser.box.setPosition(dx + 20, dy + 72);
    regUser.box.setSize({dw - 40, 32});
    regPass.box.setPosition(dx + 20, dy + 132);
    regPass.box.setSize({dw - 40, 32});
    regRole.box.setPosition(dx + 20, dy + 192);
    regRole.box.setSize({dw - 40, 32});
    regUser.draw(window);
    regPass.draw(window);
    regRole.draw(window);

    auto bOk  = makeRect(dx + dw - 200, dy + dh - 48, 85, 32, sf::Color{90,60,150});
    auto bCan = makeRect(dx + dw - 105, dy + dh - 48, 85, 32, sf::Color{90,60,60});
    window.draw(bOk);  window.draw(makeText("Register", 14, C_WHITE, dx + dw - 193, dy + dh - 40));
    window.draw(bCan); window.draw(makeText("Cancel",   14, C_WHITE, dx + dw - 90,  dy + dh - 40));

    // Tab cycle
    if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::Tab) {
        if (regUser.focused) { regUser.focused = false; regPass.focused = true; }
        else if (regPass.focused) { regPass.focused = false; regRole.focused = true; }
        else { regRole.focused = false; regUser.focused = true; }
    }
    if (e.type == sf::Event::MouseButtonPressed) {
        regUser.focused = regUser.box.getGlobalBounds().contains(
            (float)e.mouseButton.x, (float)e.mouseButton.y);
        regPass.focused = regPass.box.getGlobalBounds().contains(
            (float)e.mouseButton.x, (float)e.mouseButton.y);
        regRole.focused = regRole.box.getGlobalBounds().contains(
            (float)e.mouseButton.x, (float)e.mouseButton.y);
    }

    regUser.handleEvent(e);
    regPass.handleEvent(e);
    regRole.handleEvent(e);

    if (clicked(bOk, e)) {
        if (!regUser.buffer.empty() && !regPass.buffer.empty()) {
            Role nr = Role::USER;
            if (regRole.buffer == "ADMIN")      nr = Role::ADMIN;
            else if (regRole.buffer == "GUEST") nr = Role::GUEST;
            bool ok = auth.registerUser(regUser.buffer, regPass.buffer, nr);
            setStatus(ok ? "User '" + regUser.buffer + "' registered as " +
                           AuthSystem::roleName(nr) + "." :
                          "Error: username already exists.", !ok);
        } else setStatus("Username and password are required.", true);
        dialog = Dialog::NONE;
    }
    if (clicked(bCan, e) ||
        (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::Escape)) {
        dialog = Dialog::NONE;
        setStatus("Registration cancelled.");
    }
}
