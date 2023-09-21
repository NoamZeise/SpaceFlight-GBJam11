#include "menu.h"

#include <graphics/glm_helper.h>

const int PLAY_INDEX = 0;
const int PALETTE_INDEX = 1;
const int FULLSCREEN_INDEX = 2;
const int EXIT_INDEX = 3;
const float MENU_X = 15.0f;
const float MENU_Y = 80.0f;
const float MENU_SPACING = 18.0f;
const float MENU_DEPTH = 0.9f;

MainMenu::MainMenu(Render *render) {
    this->palettes = loadAllPalettes("textures/palettes/palettes.txt");
    this->background = render->LoadTexture("textures/menu.png");
    this->font = render->LoadFont("textures/dogicapixel.ttf");
    this->pixel = render->LoadTexture("textures/pixel.png");
    if(palettes.size() > 1)
	this->selectedPalette = palettes.size() - 2;
    else {
	if(palettes.size() < 1)
	    palettes.push_back(Palette {
		    "base", BASE_COLOUR0, BASE_COLOUR1, BASE_COLOUR2, BASE_COLOUR3});
	this->selectedPalette = 0;
    }
    bgMat = glmhelper::calcMatFromRect(glm::vec4(0, 0, GB_WIDTH, GB_HEIGHT), 0, 0.0f);
    menuText[PLAY_INDEX] = "RESUME";
    addBG(PLAY_INDEX);
    menuText[FULLSCREEN_INDEX] = "FULLSCREEN: " + std::string((isFullscreen ? "ON" : "OFF"));
    addBG(FULLSCREEN_INDEX);
    menuText[EXIT_INDEX] = "EXIT";
    addBG(EXIT_INDEX);
    updateMenuText();
}

MenuState MainMenu::Update(GbInput &input) {
    state = MenuState();
    
    if(input.press(GB::Up))
	selected = selected == 0 ? MENU_SIZE - 1 : selected - 1;
    if(input.press(GB::Down))
	selected = selected == MENU_SIZE - 1 ?  0 : selected + 1;
    if(input.press(GB::A) || input.press(GB::Right))
	switch(selected) {
	case PLAY_INDEX:
	    state.play = true;
	    break;
	case PALETTE_INDEX:
	    selectedPalette++;
	    state.paletteChanged = true;
	    break;
	case FULLSCREEN_INDEX:
	    state.fullscreen = true;
	    isFullscreen = !isFullscreen;
	    menuText[FULLSCREEN_INDEX] = "FULLSCREEN: " + std::string((isFullscreen ? "ON" : "OFF"));
	    addBG(FULLSCREEN_INDEX);
	    break;
	case EXIT_INDEX:
	    state.quit = true;
	    break;
	}

    if(input.press(GB::Left) && selected == PALETTE_INDEX) {
	selectedPalette--;
	state.paletteChanged = true;
    }

    if(state.paletteChanged) {
	if(selectedPalette < 0)
	    selectedPalette = palettes.size() - 1;
	else if(selectedPalette >= palettes.size())
	    selectedPalette = 0;
	updateMenuText();
    }
    return state;
}

void MainMenu::drawString(Render *render, std::string text,
			  float x, float y, glm::vec4 colour) {
    render->DrawString(font,
		       text,
		       glm::vec2(x, y),
		       GB_TEXT_SIZE, MENU_DEPTH,
		       colour);
}

void MainMenu::Draw(Render *render) {
    drawString(render,
	       "------------------------",
	       0.0f, MENU_Y - MENU_SPACING * 0.8f,
	       COL2);
    drawString(render,
	       "------------------------",
	       0.0f, MENU_Y + (MENU_SPACING * (MENU_SIZE - 0.5f)),
	       COL2);
    for(int i = 0; i < MENU_SIZE; i++) {
	drawString(render,
		   selected == i ?
		   "> " + menuText[i] :
		   menuText[i],
		   MENU_X, MENU_Y + MENU_SPACING * i,
		   selected == i ? COL0 : COL1);
	drawString(render,
		   "-",
		   MENU_X, MENU_Y + MENU_SPACING * (i + 0.2f),
		   selected == i ? COL0 : COL2);
    }
    render->DrawQuad(background, bgMat);
}

Palette MainMenu::getPalette() {
    return this->palettes[selectedPalette];
}

void MainMenu::updateMenuText() {
    menuText[PALETTE_INDEX] =
	  "PALETTE: < "
	+ this->palettes[selectedPalette].name
	+ " >";
    for(int i = 8; i < menuText[PALETTE_INDEX].size(); i++)
	menuText[PALETTE_INDEX][i] = toupper(menuText[PALETTE_INDEX][i]);
    addBG(PALETTE_INDEX);
}

void MainMenu::addBG(int index) {

}
