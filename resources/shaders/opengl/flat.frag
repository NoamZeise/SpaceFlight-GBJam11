#version 430

in vec2 TexCoords;
flat in int instanceID;
out vec4 colour;

const int MAX_2D_INSTANCE = 10000;

layout (std430, binding = 5) buffer perInstanceTexOffsetData
{
  vec4 texOffset[MAX_2D_INSTANCE];
};

uniform sampler2D image;
uniform vec4 spriteColour;
uniform bool enableTex;

const int COLOUR0 = int((0x1c / 255.0f) * 100);
const int COLOUR1 = int((0x52 / 255.0f) * 100);
const int COLOUR2 = int((0xa3 / 255.0f) * 100);
const int COLOUR3 = int((0xe3 / 255.0f) * 100);

uniform vec4 col0;
uniform vec4 col1;
uniform vec4 col2;
uniform vec4 col3;

void main()
{
  vec2 coord = TexCoords;
  coord.x *= texOffset[instanceID].z;
  coord.y *= texOffset[instanceID].w;
  coord.x += texOffset[instanceID].x;
  coord.y += texOffset[instanceID].y;
  if(enableTex)
      colour = texture(image, coord) * spriteColour;
  else
      colour = spriteColour;

  if(colour.w == 0)
    discard;

  int col = int(colour.r * 100);
  switch(col) {
    case COLOUR0:
      colour = col0;
      break;
  case COLOUR1:
      colour = col1;
      break;
  case COLOUR2:
      colour = col2;
      break;
  case COLOUR3:
      colour = col3;
      break;
  default:
    colour = vec4(1.0f);
  }
}
