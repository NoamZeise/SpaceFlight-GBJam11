#ifndef SYSTEM_H
#define SYSTEM_H

#include <manager.h>
#include <glm/gtc/matrix_inverse.hpp>

#include <vector>

class Planet {
 public:
    Planet(){}
    Planet(Resource::Model model, glm::mat4 modelMat);
    void Draw(Render *render);
 private:
    Resource::Model model;
    glm::mat4 modelMat;
    glm::mat4 normMat;
};


class System {
 public:
    System(){}
    System(Render *render);
    void Draw(Render *render);
 private:
    Planet sun;
    std::vector<Planet> planets;
};

#endif
