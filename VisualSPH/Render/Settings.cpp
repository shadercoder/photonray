#include "DXUT.h"
#include "Settings.h"

Settings::Settings(void)
{
}

Settings::~Settings(void)
{
}

void Settings::loadFromFile(char* fileName)
{
	ifstream fin (fileName);
	fin >> pathToFolder;
	fin >> patternString;
	fin >> firstFrame >> lastFrame >> stepFrame;
	fin >> cameraPos.x >> cameraPos.y >> cameraPos.z;
	fin >> cameraLookAt.x >> cameraLookAt.y >> cameraLookAt.z;
	fin >> screenWidth >> screenHeight;
	fin >> volumeResolution;
	string _renderState;
	fin >> _renderState;
	if (_renderState.compare(_METABALLS) == 0)
	{
		renderState = METABALLS;
	}
	else 
	if (_renderState.compare(_PARTICLES) == 0)
	{
		renderState = PARTICLES;
	}
	else 
	{
		printf("render type must be specified\n");
	}
	fin.close();
}
