#ifndef PALETTE_H
#define PALETTE_H

#include "gb_consts.h"
#include <vector>
#include <string>

// store a gameboy palette
// where smaller numbers are
// usually darker.
struct Palette {
    std::string name;
    int col0;
    int col1;
    int col2;
    int col3;
};

// given a colour in the format 0xRRGGBB,
// fill a float array with the intensity of each R, G, B
// value with a range from 0.0 - 1.0
void hexToColour(int hex, float fillCol[3]);

// given a path to an image file with 4 colours,
// horizontally across the image with even spacing,
// extract the 4 colours in the format `0xRRGGBB`
// and store in a Palette struct.
Palette loadPalette(const char *file);

// given a txt file with relative paths to palette image files,
// call loadPalette on each of these image files
std::vector<Palette> loadAllPalettes(const char* paletteListFile);

#endif
