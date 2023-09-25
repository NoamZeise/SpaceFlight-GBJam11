#include "ship_menu.h"

#include "gb_consts.h"
#include "palette.h"

#include <iostream>
#include <algorithm>

const int TARGET_MENU_POS = 0;
const int LOGS_MENU_POS = 1;
const int MENU_SIZE = 2;
const int MENU_LINE_AMOUNT = 5;
const int READING_LINE_AMOUNT = 10;

MenuMod::MenuMod(Render *render) {
    targetMod = TargetMod(render->LoadTexture("textures/ship/target-onscreen.png"),
			  render->LoadTexture("textures/ship/target-offscreen.png"));
    font = render->LoadFont("textures/dogicapixel.ttf");
    menuBg = Module(render->LoadTexture("textures/ship/menu/bg.png"),
		    glm::vec4(80, 80, 0, 0), GLASS_DEPTH + 0.2f);

    menuBg.setBaseOffset(menuOffset);
    menuBg.updateMat();
    
    menuLists[(int)MenuState::Main].items.resize(MENU_SIZE);
    menuLists[(int)MenuState::Main].items[TARGET_MENU_POS] = "Target";
    menuLists[(int)MenuState::Main].items[LOGS_MENU_POS] = "Logs";
    
    for(int i = 0; i < sizeof(SYSTEM_PLANETS) / sizeof(*SYSTEM_PLANETS); i++) {
      menuLists[(int)MenuState::Targets].items.push_back(SYSTEM_PLANETS[i].name);
    }
}

void MenuMod::controls(GbInput &input, std::vector<TargetMod> *targets) {
    if(input.press(GB::Up)) {
	menuLists[(int)state].cursorPos--;
	if(menuLists[(int)state].cursorPos < 0)
	    menuLists[(int)state].cursorPos = menuLists[(int)state].items.size() - 1;
    }
    if(input.press(GB::Down)) {
	menuLists[(int)state].cursorPos++;
	if(menuLists[(int)state].cursorPos >= menuLists[(int)state].items.size())
	    menuLists[(int)state].cursorPos = 0;
    }
    if(input.press(GB::Right)) {
	menuLists[(int)state].cursorPos+=MENU_LINE_AMOUNT;
	if(menuLists[(int)state].cursorPos > menuLists[(int)state].items.size())
	    menuLists[(int)state].cursorPos = menuLists[(int)state].items.size() - 1;
    }
    if(input.press(GB::Left)) {
	menuLists[(int)state].cursorPos-=MENU_LINE_AMOUNT;
	if(menuLists[(int)state].cursorPos < 0)
	    menuLists[(int)state].cursorPos = 0;
    }
    if(input.press(GB::A)) {
	int cursor = menuLists[(int)state].cursorPos;
	switch(state) {
	case MenuState::Main:
	    switch(cursor) {
	    case TARGET_MENU_POS:
		state = MenuState::Targets;
		break;
	    case LOGS_MENU_POS:
		state = MenuState::Logs;
		break;
	    }
	    break;
	    
	case MenuState::Logs:
	    if(logs.size() > 0) {
		state = MenuState::ReadingLog;
		menuLists[(int)state].cursorPos = 0;
		menuLists[(int)state].items = logs[cursor].text;
		if(menuLists[(int)MenuState::Logs].items[cursor].substr(0, 3) == "[N]") {
		    menuLists[(int)MenuState::Logs].items[cursor] =
			menuLists[(int)MenuState::Logs].items[cursor].substr(3);
		}
		readingTimer = 0;
	    }
	    break;
	case MenuState::ReadingLog:
	    menuLists[(int)state].cursorPos += READING_LINE_AMOUNT;
	    if(menuLists[(int)state].cursorPos >= menuLists[(int)state].items.size()) {
		menuLists[(int)state].cursorPos = menuLists[(int)state].items.size() - 1;
		state = MenuState::Logs;
		readingTimer = 0;
	    }
	    break;
	case MenuState::Targets:
	    glm::vec3 target = SYSTEM_PLANETS[cursor].pos;
	    bool found = false;
	    for(int i = 0; i < targets->size(); i++)
		if((*targets)[i].getTarget() == target) {
		    found = true;
		    (*targets).erase((*targets).begin() + i--);
		    menuLists[(int)state].items[cursor].erase(0, 1);
		    break;
		}
	    if(!found) {
		(*targets).push_back(targetMod);
		(*targets).back().setTarget(target);
		menuLists[(int)state].items[cursor].insert(0, ">");
	    }
	    break;
	}
    }
    if(input.press(GB::B)) {
	switch(state) {
	case MenuState::Main:
	    toExit = true;
	    break;
	case MenuState::ReadingLog:
	    state = MenuState::Logs;
	    readingTimer = 0;
	    break;
	case MenuState::Targets:
	case MenuState::Logs:
		state = MenuState::Main;
	    break;
	}
    }
}

void MenuMod::Update(GbInput &input, gamehelper::Timer &timer, std::vector<TargetMod> *targets) {
    bool timerChanged = false;
    if(showingMenu) {
	if(menuTimer < menuTimeLim) {
	    menuTimer += timer.FrameElapsed();
	    timerChanged = true;
	}
	controls(input, targets);
    }
    else if(menuTimer > 0) {
	menuTimer -= timer.FrameElapsed();
	timerChanged = true;
    }

    if(readingTimer < menuTimeLim) {
	readingTimer += timer.FrameElapsed();
	timerChanged = true;
	if(state == MenuState::ReadingLog)
	    scaleFactor = 1 + 0.8*(readingTimer / menuTimeLim);
	else
	    scaleFactor = 1 + 0.8*(1 - (readingTimer / menuTimeLim));
    }
    
    if(timerChanged) {
	glm::vec4 menuPos = menuBg.getPos();
	glm::vec2 dim = menuBg.getDim();
	menuPos.z = dim.x * scaleFactor;
	menuPos.w = dim.y *scaleFactor;
	float ratio = 1 - (menuTimer / menuTimeLim);
	ratio *= ratio;
	menuOffset = glm::vec2((int)(ratio * (menuPos.z + 2)) - ((scaleFactor - 1)*dim.x),
			       (int)(ratio * (menuPos.w / 2)) - ((scaleFactor - 1)*dim.y)
			       );
	menuBg.setBaseOffset(menuOffset);
	menuBg.setPos(menuPos);
    }
    
    menuBg.Update(timer);
}

void MenuMod::Draw(Render *render) {
    menuBg.Draw(render);
    int cursor = menuLists[(int)state].cursorPos;
    int listSize = MENU_LINE_AMOUNT;
    if(state == MenuState::ReadingLog) {
	listSize = READING_LINE_AMOUNT;
	//if(readingTimer < menuTimeLim/2)
	//   listSize = 1;
    }
    int menuStart = (cursor / listSize) * listSize;
    for(int i = menuStart; i < menuLists[(int)state].items.size()
	    && i < menuStart + listSize; i++) {
	render->DrawString(font, menuLists[(int)state].items[i] +
			   (menuLists[(int)state].cursorPos == i ?
			    "<" : ""),
			   glm::vec2(85 + menuOffset.x,
				     93 + menuOffset.y + (i - menuStart) * 10),
			   1, GLASS_DEPTH + 0.21f, COL1);
    }
}

bool MenuMod::menuInTransition() {
    if(!showingMenu)
	return menuTimer > 0;
    return menuTimer < menuTimeLim;
}

void MenuMod::toggleMenu() {
    toExit = false;
    showingMenu = !showingMenu;
    if(!showingMenu && menuTimer > menuTimeLim)
	menuTimer = menuTimeLim;
    if(showingMenu && menuTimer < 0)
	menuTimer = 0;
}

void MenuMod::addLog(SystemLog log, bool read) {
    logs.push_back(log);
    std::string newLog = logs.back().title;
    menuLists[(int)MenuState::Logs].items.push_back("");

    std::sort(logs.begin(), logs.end(), [](SystemLog l1, SystemLog l2) {
	if(l1.title.substr(0, 3) == l2.title.substr(0, 3)) {
	    if(l1.title.size() > l2.title.size())
		return false;
	    if(l1.title.size() < l2.title.size())
		return true;
	}
	return l1.title < l2.title;
    });
    for(int i = 0; i < logs.size(); i++) {
	menuLists[(int)MenuState::Logs].items[i] =
	    (logs[i].title == newLog && !read ? "[N]" : "") +
	    logs[i].title;
    }
}
