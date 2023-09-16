#ifndef PALETTE_H
#define PALETTE_H

#include "gb_consts.h"
#include <vector>
#include <string>

struct Palette {
    std::string name;
    int col0;
    int col1;
    int col2;
    int col3;
};

inline float extractHexChannel(int hex, int index) {
    return (float)((hex >> 8 * index) & 0xFF) / 255.0f;
}

inline void hexToColour(int hex, float fillCol[3]) {
    for(int i = 0; i < 3; i++)
	fillCol[i] = extractHexChannel(hex, i);
}

std::vector<Palette> loadAllPalettes(const char* paletteListFile);

Palette loadPalette(const char* file);

#endif
