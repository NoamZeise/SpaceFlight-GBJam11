#ifndef SHIP_MENU_H
#define SHIP_MENU_H

#include "system_consts.h"
#include "ship_modules.h"
#include "controls.h"
#include "logs.h"

class MenuMod {
public:
    MenuMod() {}
    MenuMod(Render *render);
    void Update(GbInput &input, gamehelper::Timer &timer, std::vector<TargetMod> *targets);
    void Draw(Render *render);
    void toggleMenu();
    bool menuInTransition();
    bool isExit() { return toExit; }
    void addLog(SystemLog log, bool read);
    TargetMod getTargetMod() { return targetMod; }
private:

    void controls(GbInput &input, std::vector<TargetMod> *targets);
    
    Resource::Font font;

    enum class MenuState {
	Main,
	Targets,
	ReadingLog,
	Logs,
    };

    MenuState state = MenuState::Main;

    struct MenuList {
	std::vector<std::string> items;
	int cursorPos = 0;
    };
    
    MenuList menuLists[(int)MenuState::Logs + 1];

    TargetMod targetMod;

    Module menuBg;

    std::vector<SystemLog> logs;

    bool toExit = false;
    bool showingMenu = false;
    float menuTimer = -1;
    float menuTimeLim = 300;

    float readingTimer = 300;
    
    glm::vec2 menuOffset = glm::vec2(100, 100);
    float scaleFactor = 1;
};


#endif
