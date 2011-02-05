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

	const vector<Particle>& getParticles() const;
	int getParticlesCount() const;

private:
	const int DIMENSIONS; // length of input file line (in words)
	vector<Particle> particleBuff;
	
	int particleCount;
	int curr, first, last, step;
	FILE* pIn;

	void open(int cur);
	void close();
	
	const char* pathToFolder;
	const char* filePattern;
};
