#ifndef GAME_LOGS_H
#define GAME_LOGS_H

#include <vector>
#include <string>
#include <glm/glm.hpp>

struct SystemLog {
    glm::vec3 pos;
    std::vector<std::string> text;
    std::string title = "";
    bool found = false;
};

std::vector<SystemLog> getLogs();

glm::vec3 getPlayerSpawn();

#endif
