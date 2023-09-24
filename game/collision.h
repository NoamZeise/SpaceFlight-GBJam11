#ifndef COLLISION_H
#define COLLISION_H

#include <glm/glm.hpp>

enum class CollisionState { Collide, Close, None };

struct Collision {
    CollisionState state;
    glm::vec3 pos;
};

#endif
