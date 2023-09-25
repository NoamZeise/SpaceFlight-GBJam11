#include "logs.h"

#include <fstream>
#include <iostream>

std::vector<SystemLog> logs;
glm::vec3 playerSpawn = glm::vec3(600, 0, 0);

std::string logPath = "logs/";

const int LINE_WIDTH = 22;

SystemLog getLog(std::ifstream &log) {
    SystemLog l;
    std::string line;
    int i = 0;
    while(std::getline(log, line)) {
	switch(i++) {
	case 0:
	    l.pos.x = atof(line.c_str());
	    break;
	case 1:
	    l.pos.y = atof(line.c_str());
	    break;
	case 2:
	    l.pos.z = atof(line.c_str());
	    break;
	case 3:
	    l.title = line;
	default:
	    while(line.size() > LINE_WIDTH) {
		bool rem = false;
		for(int i = LINE_WIDTH - 1; i > 0; i--) {
		    if(line[i] == ' ') {
			l.text.push_back(line.substr(0, i));
			rem = true;
			line = line.substr(i + 1);
			break;
		    }
		}
		if(!rem) {
		    l.text.push_back(
			    line.substr(0, LINE_WIDTH - 1));
		    line = line.substr(LINE_WIDTH - 1);
		}
	    }
	    if(line.size() > 0)
		l.text.push_back(line);
	}
    }
    log.close();
    return l;
}

std::vector<std::string> getFound() {
    std::ifstream save("ship.save");
    std::vector<std::string> found;
    if(save.good()) {
	bool firstLine = true;
	std::string line;
	std::getline(save, line);
	int space = line.find(' ');
	playerSpawn.x = atof(line.substr(0, space).c_str());
	int pSpace = space + 1;
	space = line.find(' ', pSpace);
	playerSpawn.y = atof(line.substr(pSpace, space).c_str());
	pSpace = space + 1;
	space = line.find(' ', pSpace);
	playerSpawn.z = atof(line.substr(pSpace, space).c_str());
	while(std::getline(save, line))
		found.push_back(line);
	save.close();
    }
    return found;
}

std::vector<SystemLog> getLogs() {
    if(!logs.empty())
	return logs;
    std::vector<std::string> found = getFound();
    int i = 1;
    while(true) {
	std::ifstream f(logPath + std::to_string(i++) + ".log");
	if(f.good()) {
	    auto log = getLog(f);
	    if(!found.empty()) {
		log.found = true;
		for(auto& f: found)
		    if(log.title == f)
			log.found = false;
	    }
	    logs.push_back(log);
	}
	else
	    break;
    }
    return logs;
}

glm::vec3 getPlayerSpawn() {
    return playerSpawn;
}
