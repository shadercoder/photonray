#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <vector>
#include "Particle.h"

using namespace std;

class ParticlesContainer 
{
public:
	ParticlesContainer(void);
	void init(string& _pathToFolder, string& _filePattern, int _first, int _last, int _step);
	~ParticlesContainer(void);
	Particle* getFrame(int num);
	Particle* getNextFrame();
	int getNumCurrFrame();

	//const Particle* getParticles() const;
	const vector<Particle>& getParticles() const;
	int getParticlesCount() const;

private:
//	float data[MAX_PARTICLES][DIMENSIONS];
	//Particle particleBuff[MAX_PARTICLES];	
	const int DIMENSIONS;
	vector<Particle> particleBuff;
	
	int particleCount;
	int curr, first, last, step;
	FILE* pIn;

	void open(int cur);
	void close();
	
	const char* pathToFolder;
	const char* filePattern;
};
