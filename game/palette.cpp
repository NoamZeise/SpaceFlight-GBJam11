#include "palette.h"

#include <resource_loader/stb_image.h>
#include <logger.h>
#include <fstream>

std::string getPaletteName(const char* file) {
    std::string palName = std::string(file);
    int last_slash = 0, last_point = 0;
    for(int i = palName.length() - 1; i >= 0; i--) {
	if(palName[i] == '/') {
	    last_slash = i + 1;
	    break;
	}
	if(palName[i] == '.' && last_point == 0)
	    last_point = i;
    }
    return palName.substr(last_slash, last_point - last_slash);
}

void hexToColour(int hex, float fillCol[3]) {
    for(int i = 0; i < 3; i++) {
	printf("%x \n", hex);
	printf("%x \n", hex >> 8);
	printf("%x \n", (hex >> 8 * i) & 0xFF);
	fillCol[i] = (float)((hex >> 8 * (2-i)) & 0xFF) / 255;
    }
}

glm::vec4 hexToColour(int hex) {
    float col[3];
    hexToColour(hex, col);
    LOG("col0: " << col[0]);
    LOG("col1: " << col[1]);
    LOG("col2: " << col[2]);
    return glm::vec4(col[0], col[1], col[2], 1.0f);
}

ShaderPalette Palette::toShaderPalette() {
    ShaderPalette pal;
    pal.col0 = hexToColour(this->col0);
    pal.col1 = hexToColour(this->col1);
    pal.col2 = hexToColour(this->col2);
    pal.col3 = hexToColour(this->col3);
    return pal;
}

Palette loadPalette(const char* file) {
    Palette pal {
	getPaletteName(file),
	0, 0, 0, 0,
    };
    int w, h, nrCh;
    w = 0;
    h = 0;
    nrCh = 0;
    stbi_uc* im = stbi_load(file, &w, &h, &nrCh, 0);
    if(!im) {
	LOG_ERROR("Failed to load palette image from file, "
		  "returning blank palette!");
	pal.name += " (failed to load)";
	return pal;
    }
    if(w < 4 || h < 1 || nrCh < 3) {
	LOG_ERROR("loaded palette had unsupported dimensions or channels,"
		  " returning blank palette!");
	pal.name += " (failed to load)";
	return pal;
    }
    int step = w / 4;
    for(int x = 0; x < w; x+=step) {
	int* p = nullptr;
	int col = x / step;
	switch(col) {
	case 0:
	    p = &pal.col0;
	    break;
	case 1:
	    p = &pal.col1;
	    break;
	case 2:
	    p = &pal.col2;
	    break;
	case 3:
	    p = &pal.col3;
	    break;
	default:
	    LOG_ERROR("Palette step error, returning partial palette.");
	    pal.name += " (failed to load)";
	    return pal;
	}
	*p = 0;
	for(int ch = 0; ch < 3; ch++)
	    *p |= im[x*nrCh + ch] << 8 * (2 - ch);
	printf("col: \%x \n", *p);
    }
    stbi_image_free(im);
    LOG("loaded palette: " << file);
    return pal;
}

std::vector<Palette> loadAllPalettes(const char* paletteListFile) {
    //get line count for preallocating vec
    std::string path = paletteListFile;
    int last_dash = 0;
    for(int i = path.length() - 1; i >= 0 ; i--) {
	if(path[i] == '/') {
	    last_dash = i;
	    break;
	}
    }
    if(last_dash != 0)
	path = path.substr(0, last_dash + 1);
    else
	path = "";
    std::ifstream palList(paletteListFile);
    std::string line;
    std::vector<Palette> palettes;
    while(std::getline(palList, line))
	palettes.push_back(loadPalette((path + line).c_str()));
    return palettes;
}
