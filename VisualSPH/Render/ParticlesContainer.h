#pragma once
#include <cstdio>
#include <string>
#include <fstream>
#include "Particle.h"

using namespace std;

const int MAX_PARTICLES = 1 << 14;
const int DIMENSIONS = 8;

// Input Stream Particle System 
class ParticlesContainer 
{
public:
	ParticlesContainer(void);
	void init(string& _pathToFolder, string& _filePattern, int _first, int _last, int _step);
	~ParticlesContainer(void);
	Particle* getFrame(int num);
	Particle* getNextFrame();
	int getNumCurrFrame();

	const Particle* getParticles() const;
	int getParticlesCount() const;

private:
//	float data[MAX_PARTICLES][DIMENSIONS];
	Particle particleBuff[MAX_PARTICLES];
	
	int particleCount;
	int curr, first, last, step;
	FILE* pIn;

	void open(int cur);
	void close();
	
	const char* pathToFolder;
	const char* filePattern;
};
