#ifndef GAME_LOGS_H
#define GAME_LOGS_H

#include <vector>
#include <string>
#include <glm/glm.hpp>

struct SystemLog {
    glm::vec3 pos;
    std::vector<std::string> text;
    std::string title = "";
};

std::vector<SystemLog> getLogs();

#endif
