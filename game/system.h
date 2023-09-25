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
    virtual void rotate(float a);
    
    std::string name;
    Resource::Model model;
    glm::mat4 modelMat;
    glm::mat4 normMat;
    float speed = 0.0f;
    float radius;
    glm::vec3 pos;
    glm::vec3 axisOfRot = glm::vec3(0, 0, 1);
};

struct LogModel : public Planet {
    LogModel() : Planet() {};
    LogModel(Resource::Model model, glm::vec3 pos);
};

class System {
 public:
    System(){}
    System(Render *render);

    void Draw(Render *render);
    void Update(gamehelper::Timer &timer);
    Collision planetCollisions(glm::vec3 pos);
    Collision logCollisions(glm::vec3 pos);
    
 private:
    std::vector<Planet> planets;
    std::vector<LogModel> logs;
};

#endif
