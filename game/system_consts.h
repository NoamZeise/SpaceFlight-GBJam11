#ifndef SYSTEM_CONSTS_H
#define SYSTEM_CONSTS_H

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

struct MapObject {
    std::string name;
    std::string tex;
    glm::vec3 pos;
    float radius;
    float speed;
    bool shading = true;
    float initalRot = 0;
};

const MapObject SYSTEM_PLANETS[] = {
    MapObject{"sun", "", glm::vec3(0), 150, 0.0001f, false},

    MapObject{"kasi", "textures/Planet/earthlike.png", glm::vec3(1100, 300, 0),
              40.5, 0.01f},

    MapObject{"gaia", "textures/Planet/earthlike.png", glm::vec3(-1400, 200, 0),
              40.5, 0.01f},
    MapObject{"diana", "textures/Planet/moon.png", glm::vec3(-1400, 50, 10),
              6.5, 0.001f},

    MapObject{"ripel-1", "textures/Planet/triangles.png",
              glm::vec3(890, 1640, -20), 30.5, 0.005f},
    MapObject{"ripel-2", "textures/Planet/triangles.png",
              glm::vec3(950, 1616, 20), 23.1, 0.005f, true,
              glm::pi<float>() / 2},

    MapObject{"sila", "textures/Planet/planet1.png",
              glm::vec3(4080, -100, -207), 50, 0.001f},
    MapObject{"sila-1", "textures/Planet/planet1.png",
              glm::vec3(4010, -10, -100), 7, 0.01f, true, 1.0f},
    MapObject{"sila-2", "textures/Planet/earthlike.png",
              glm::vec3(4250, -180, -207), 9, 0.03f},
    MapObject{"sila-3", "textures/Planet/moon.png", glm::vec3(4270, -190, -207),
              4, 0.03f},

    MapObject{"RW-1011", "textures/Planet/metal-planet.png",
              glm::vec3(120, -440, -200), 30, 0.3f},
    MapObject{"RW-998", "textures/Planet/metal-planet.png",
              glm::vec3(120, -440, -140), 10, 0.9f},
    MapObject{"RW-999", "textures/Planet/metal-planet.png",
              glm::vec3(120, -440, -260), 10, 0.9f},

    MapObject{"Waxa", "textures/Planet/noise.png", glm::vec3(-1000, -1500, 450),
              23, 0.04f},

    MapObject{"VenA", "textures/Planet/organic1.png",
              glm::vec3(-3500, 3900, 200), 73, 0, true, 2.0f},
    MapObject{"VenB", "textures/Planet/organic1.png",
              glm::vec3(-3480, 3980, 200), 43, 0},

    MapObject{"link", "textures/Planet/lines.png", glm::vec3(-1200, 1300, -400),
              27, 0.01f},
    MapObject{"link-1", "textures/Planet/moon.png",
              glm::vec3(-1170, 1330, -320), 5, 0.01f},
    MapObject{"link-2", "textures/Planet/metal-planet.png",
              glm::vec3(-1270, 1310, -400), 4, 0.6f},
};

#endif
