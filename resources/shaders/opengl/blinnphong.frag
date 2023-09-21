#version 430

layout (location = 0) in vec2 inTexCoords;
layout (location = 1) in vec3 inFragPos;
layout (location = 2) in vec3 inNormal;

out vec4 outColour;

uniform sampler2D image;
uniform vec4 spriteColour;
uniform bool enableTex;
uniform bool useShading;

const int COLOUR0 = int((0x1c / 255.0f) * 100);
const int COLOUR1 = int((0x52 / 255.0f) * 100);
const int COLOUR2 = int((0xa3 / 255.0f) * 100);
const int COLOUR3 = int((0xe3 / 255.0f) * 100);

uniform vec4 col0;
uniform vec4 col1;
uniform vec4 col2;
uniform vec4 col3;

struct LightingParams
{
  vec4 ambient;
  vec4 diffuse;
  vec4 specular;
  vec4 direction;
  vec4 camPos;
};

uniform LightingParams lighting;

void main()
{
  vec4 objectColour;
  objectColour = texture(image, inTexCoords) * spriteColour * int(enableTex) +
    int(!enableTex) * spriteColour;

  vec3 ambient = lighting.ambient.xyz * lighting.ambient.w;

  vec3 normal = normalize(inNormal);
  vec3 lightDir = normalize(-inFragPos);

  float lambertian = max(dot(normal, lightDir), 0.0) * int(useShading)
    + 0.7 * int(!useShading);
  vec3 diffuse = lighting.diffuse.xyz * lighting.diffuse.w * lambertian;

  float specularIntensity = 0.0;
  if(lambertian > 0.0)
  {
    vec3 viewDir = normalize(lighting.camPos.xyz - inFragPos);

    vec3 halfDir = normalize(lightDir + viewDir);
    specularIntensity = pow(max(dot(normal, halfDir), 0.0), lighting.specular.w);
  }
  vec3 specular = lighting.specular.xyz * specularIntensity;

  outColour = vec4(ambient + diffuse + specular, 1.0) * objectColour;

  float intensity = (outColour.x + outColour.y + outColour.z) * 33;
  float alpha = outColour.w;
  if(intensity < COLOUR0)
    outColour = col0;
  else if(intensity < COLOUR1)
    outColour = col1;
  else if(intensity < COLOUR2)
    outColour = col2;
  else
    outColour = col3;
  outColour.w = alpha;
}
