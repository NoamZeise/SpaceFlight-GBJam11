#ifndef SYSTEM_H
#define SYSTEM_H

#include <manager.h>
#include <glm/gtc/matrix_inverse.hpp>

#include <vector>

#include "collision.h"

struct Planet {
    Planet(){}
    Planet(std::string name, Resource::Model model, glm::vec3 pos, float radius, float speed);    
    void Draw(Render *render);
    void Update(gamehelper::Timer &timer);
    void rotate(float a);
    
    std::string name;
    Resource::Model model;
    glm::mat4 modelMat;
    glm::mat4 normMat;
    float speed = 0.0f;
    float radius;
    glm::vec3 pos;
};

class System {
 public:
    System(){}
    System(Render *render);
    void Draw(Render *render);
    void Update(gamehelper::Timer &timer);
    Collision planetCollisions(glm::vec3 pos);
    
 private:
    Planet sun;
    std::vector<Planet> planets;
};

#endif
