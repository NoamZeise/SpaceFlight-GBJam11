#ifndef SYSTEM_H
#define SYSTEM_H

#include <manager.h>
#include <glm/gtc/matrix_inverse.hpp>

#include <vector>

class Planet {
 public:
    Planet(){}
    Planet(Resource::Model model, glm::vec3 pos, float radius, float speed);    
    void Draw(Render *render);
    void Update(gamehelper::Timer &timer);
 private:
    Resource::Model model;
    glm::mat4 modelMat;
    glm::mat4 normMat;
    float speed = 0.0f;
};


class System {
 public:
    System(){}
    System(Render *render);
    void Draw(Render *render);
    void Update(gamehelper::Timer &timer);
 private:
    Planet sun;
    std::vector<Planet> planets;
};

#endif
