#version 450

layout(set = 2, binding = 0) uniform sampler texSamp;
layout(set = 2, binding = 1) uniform texture2D textures[30];

const int COLOUR0 = int((0x1c / 255.0f) * 100);
const int COLOUR1 = int((0x52 / 255.0f) * 100);
const int COLOUR2 = int((0xa3 / 255.0f) * 100);
const int COLOUR3 = int((0xe3 / 255.0f) * 100);

struct per2DFragData
{
    vec4 colour;
    vec4 texOffset;
    uint texID;
};

layout(std140, set = 3, binding = 0) readonly buffer PerInstanceBuffer {
    per2DFragData data[];
} pib;

layout(set = 4, binding = 0) uniform UBO {
  vec4 col0;
  vec4 col1;
  vec4 col2;
  vec4 col3;
} palette;

layout(location = 0) in vec3 inTexCoord;

layout(location = 0) out vec4 outColour;

vec4 calcColour(vec4 texOffset, vec4 colour, uint texID) {
  vec2 coord = inTexCoord.xy;
  coord.x *= texOffset.z;
  coord.y *= texOffset.w;
  coord.x += texOffset.x;
  coord.y += texOffset.y;

  vec4 col = texture(sampler2D(textures[texID], texSamp), coord) * colour;
  
  if(col.w == 0)
    discard;
  return col;
}

void main() {
  uint index = uint(inTexCoord.z);
  outColour = calcColour(pib.data[index].texOffset, pib.data[index].colour, pib.data[index].texID);
  int col = int(outColour.r * 100);
  switch(col) {
    case COLOUR0:
      outColour = palette.col0;
      break;
  case COLOUR1:
      outColour = palette.col1;
      break;
  case COLOUR2:
      outColour = palette.col2;
      break;
  case COLOUR3:
      outColour = palette.col3;
      break;
  default:
    outColour = vec4(1.0f);
  }
}
