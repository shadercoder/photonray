#pragma once
#include "d3dUtil.h"
#include <string>
#include <fstream>

using namespace std;

/*
<path to folder string>
<file pattern string>
<first frame (inclusive) int>
<last frame (inclusive) int>
<frame step int>
<camera position (x,y,z) float>
<camera look at (x,y,z) float>
<screen resolution (width, height) int>
*/

#define _METABALLS "metaballs"
#define METABALLS 1
#define _PARTICLES "particles"
#define PARTICLES 2

class Settings
{
public:
	int firstFrame, lastFrame, stepFrame;
	string pathToFolder;
	string patternString;
	D3DXVECTOR3 cameraPos;
	D3DXVECTOR3 cameraLookAt;
	int screenWidth, screenHeight;
	int volumeResolution;
	void loadFromFile(char* fileName);
	int renderState;
	Settings(void);
	~Settings(void);
};
