#include "palette.h"

#include <resource_loader/stb_image.h>
#include <logger.h>

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
	return pal;
    }
    if(w < 4 || h < 1 || nrCh < 3) {
	LOG_ERROR("loaded palette had unsupported dimensions or channels,"
		  " returning blank palette!");
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
	    return pal;
	}
	*p = 0;
	for(int ch = 0; ch < 3; ch++)
	    *p |= im[x*nrCh + ch] << 8 * (2 - ch);
	printf("%d: %x, ", col, *p);
    }
    stbi_image_free(im);
    LOG("\nloaded palette: " << file);
    return pal;
}

std::vector<Palette> loadAllPalettes(const char* paletteListFile) {
    //get line count for preallocating vec
    std::vector<Palette> palettes;

    
    return palettes;
}
