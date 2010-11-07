#pragma once
#include <fstream>
#include <string>

class Settings
{
public:
	float cx, cy, cz;
	float lx, ly, lz;
	std::string path;
	std::string pattern;
	bool fullscreen;
	int screendefinition[2];
	int firstFrame, lastFrame, stepFrame;
	Settings();
}

