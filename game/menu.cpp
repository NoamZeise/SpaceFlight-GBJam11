#include "menu.h"

MainMenu::MainMenu(std::vector<Palette> palettes, Resource::Font font) {
    this->palettes = palettes;
    this->font = font;
    if(palettes.size() > 1)
	this->selectedPalette = palettes[palettes.size() - 2];
    else {
	if(palettes.size() < 1)
	    palettes.push_back(Palette {
		    "base", colour0, colour1, colour2, colour3});
	this->selectedPalette = palettes[0];
    }
}

void MainMenu::Update(GbInput &input) {
    
}

void MainMenu::Draw(Render *render) {

}

MenuState MainMenu::getState() {
    return state;
}

Palette MainMenu::getPalette() {
    return this->selectedPalette;
}
