#ifndef MENU_H
#define MENU_H

#include <render.h>
#include "palette.h"
#include "controls.h"

// member will be true if it was selected last frame
struct MenuState {
    bool play = false;
    bool paletteChanged = false;
    bool fullscreen = false;
    bool quit = false;
};

const int MENU_SIZE = 4;

class MainMenu {
 public:
    MainMenu(){}
    MainMenu(Render *render);
    MenuState Update(GbInput &input);
    void Draw(Render *render);
    Palette getPalette();
 private:
    void updateMenuText();
    void addBG(int index);
    void drawString(Render *render, std::string text,
		    float x, float y, glm::vec4 colour);
    
    Resource::Texture background;
    glm::mat4 bgMat;
    Resource::Font font;
    Resource::Texture pixel;
    std::vector<Palette> palettes;
    int selectedPalette;
    MenuState state;
    unsigned int selected = 0;
    std::string menuText[MENU_SIZE];
    glm::mat4 menuBG[MENU_SIZE];
    bool isFullscreen = false;
};


#endif
