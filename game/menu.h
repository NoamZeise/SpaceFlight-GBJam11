#ifndef MENU_H
#define MENU_H

#include <render.h>
#include "palette.h"
#include "controls.h"

// member will be true if it was selected last frame
struct MenuState {
    bool play = false;
    bool paletteChanged = false;
    bool quit = false;
};

class MainMenu {
 public:
    MainMenu(){}
    MainMenu(std::vector<Palette> palettes, Resource::Font font);
    void Update(GbInput &input);
    void Draw(Render *render);
    MenuState getState();
    Palette getPalette();
 private:
    Resource::Font font;
    std::vector<Palette> palettes;
    Palette selectedPalette;
    MenuState state;
};


#endif
