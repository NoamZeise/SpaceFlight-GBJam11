#ifndef GB_CONSTS_H
#define GB_CONSTS_H

#define GB_WIDTH 160
#define GB_HEIGHT 144

#define GB_TEXT_SIZE 1.0f

const float GLASS_DEPTH = 0.0f;
const float THROTTLE_LIM = 1.0;
const float NEAR_CLIP_3D = 0.5f;
const float FAR_CLIP_3D = 100000000000000.0f;

// matches flat.frag shader
const int BASE_COLOUR0 = 0x1c1c1c;
const int BASE_COLOUR1 = 0x525252;
const int BASE_COLOUR2 = 0xa3a3a3;
const int BASE_COLOUR3 = 0xe3e3e3;

#endif
