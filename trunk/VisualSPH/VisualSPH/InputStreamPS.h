#pragma once
#include "particlesystem.h"
#include "Common\d3dUtil.h"
#include "gParticlesContainer.h"
#include <cstdio>
#include <string>

using namespace std;

const int MAX_PARTICLES = 1 << 13;
const int DIMENSIONS = 8;

// Input Stream Particle System 
class InputStreamPS :
	public ParticleSystem
{
public:
	InputStreamPS(void);
	void init(ID3D10Device* device, string& _pathToFolder, string& _filePattern, int _first, int _last, int _step);
	~InputStreamPS(void);
	Particle* getFrame(int num);
	Particle* getNextFrame();
	int getNumCurrFrame();
	void drawAll();
private:
	float data[MAX_PARTICLES][DIMENSIONS];
	Particle particleBuff[MAX_PARTICLES];
	gParticlesContainer particleView;
	ID3D10Device* device;
	int particleCount;
	int curr, first, last, step;
	FILE* pIn;

	void open(int cur);
	void close();

	
	const char* pathToFolder;
	const char* filePattern;
};
