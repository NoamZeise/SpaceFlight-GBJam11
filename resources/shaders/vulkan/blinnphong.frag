#version 450

layout(push_constant) uniform fragconstants
{
    vec4 colour;
    vec4 texOffset;

    ivec4 props;
    // [0] - int - texID
    //[1] - bool - no shading
} pc;

layout(set = 3, binding = 0) uniform sampler texSamp;
layout(set = 3, binding = 1) uniform texture2D textures[20];
layout(set = 4, binding = 0) uniform LightingUBO
{
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec4 direction;
    vec4 camPos;
} lighting;

layout(set = 5, binding = 0) uniform UBO {
  vec4 col0;
  vec4 col1;
  vec4 col2;
  vec4 col3;
} palette;

layout(location = 0) in vec2 inTexCoord;
layout(location = 1) in vec3 inFragPos;
layout(location = 2) in vec3 inNormal;

layout(location = 0) out vec4 outColour;

const int COLOUR0 = int((0x1c / 255.0f) * 100);
const int COLOUR1 = int((0x52 / 255.0f) * 100);
const int COLOUR2 = int((0xa3 / 255.0f) * 100);
const int COLOUR3 = int((0xe3 / 255.0f) * 100);


void main()
{
    vec2 coord = inTexCoord.xy;
    coord.x *= pc.texOffset.z;
    coord.y *= pc.texOffset.w;
    coord.x += pc.texOffset.x;
    coord.y += pc.texOffset.y;
    
    bool useTex = bool(pc.texID); //ID zero == no tex
    vec4 objectColour =
      int(useTex) *
      texture(sampler2D(textures[pc.texID], texSamp), coord) *
      pc.colour
      + int(!useTex) * pc.colour;

    vec3 ambient = lighting.ambient.xyz * lighting.ambient.w;

    vec3 normal = normalize(inNormal);

    //-inFragPos;
    vec3 lightDir = normalize(-inFragPos);//normalize(-lighting.direction.xyz);

    float lambertian = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = lighting.diffuse.xyz * lighting.diffuse.w * lambertian;
    
    vec3 viewDir = normalize(lighting.camPos.xyz - inFragPos);
    vec3 halfDir = normalize(lightDir + viewDir);
    bool useSpec = lambertian > 0.0;
    float specularIntensity = int(useSpec) *
      pow(max(dot(normal, halfDir), 0.0), lighting.specular.w);

    vec3 specular = lighting.specular.xyz * specularIntensity;
    
    outColour = vec4(ambient + diffuse + specular, 1.0) * objectColour;

    // limit to 4 colours
    float intensity = (outColour.x + outColour.y + outColour.z) * 33;
    float alpha = outColour.w;
    if(intensity < COLOUR0)
      outColour = palette.col0;
    else if(intensity < COLOUR1)
      outColour = palette.col1;
    else if(intensity < COLOUR2)
      outColour = palette.col2;
    else
      outColour = palette.col3;
    outColour.w = alpha;
}
