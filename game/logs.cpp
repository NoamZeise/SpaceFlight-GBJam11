#include "logs.h"

#include <fstream>
#include <iostream>

std::vector<SystemLog> logs;

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


std::vector<SystemLog> getLogs() {
    if(!logs.empty())
	return logs;
    int i = 1;
    while(true) {
	std::ifstream f(logPath + std::to_string(i++) + ".log");
	if(f.good())
	    logs.push_back(getLog(f));
	else
	    break;
    }
    return logs;
}
