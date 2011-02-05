#include "DXUT.h"
#include "ParticlesContainer.h"

void ParticlesContainer::init(string& _pathToFolder, string& _filePattern, int _first, int _last, int _step)
{
	pathToFolder = _pathToFolder.c_str();
	filePattern = _filePattern.c_str();
	first = _first;
	last = _last;
	step = _step;
	curr = first;
	particleBuff.resize(1 << 12);
	getFrame(first);
}

ParticlesContainer::ParticlesContainer(void): DIMENSIONS(8)
{
}


ParticlesContainer::~ParticlesContainer(void)
{
}

void ParticlesContainer::open(int curr)
{
	char fileName[50];
	sprintf_s(fileName, 50, "%s%05d%s", pathToFolder, curr, filePattern);
	errno_t err;
	err = fopen_s(&pIn, fileName, "r");
	if (err != 0)
	{
		//TODO: add exception
		return;
	}
}
void ParticlesContainer::close()
{
	fclose(pIn);
}
Particle* ParticlesContainer::getFrame(int num)
{
	open(num);
	particleBuff.clear();
	// skip header
	char junk[128];
	for(int i = 0; i < DIMENSIONS; ++i)
	{
		fscanf_s(pIn, "%s", junk, _countof(junk));
	}
	int sz = 0;
	float data[8] = {};
	Particle current;	
	for(;fscanf_s(pIn, "%f %f %f %f %f %f %f %f", &data[0], &data[1], &data[2], &data[3], &data[4], &data[5], &data[6], &data[7]) == DIMENSIONS; ++sz)
	{
		current = Particle(data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);
		particleBuff.push_back(current);
	}
	particleCount = sz;
	close();
	return (&particleBuff[0]);
}

Particle* ParticlesContainer::getNextFrame()
{
	if ((curr + step) > last)
	{
		curr = first;
	} 
	else
	{
		curr += step;
	}	
	return getFrame(curr);
}

int ParticlesContainer::getNumCurrFrame()
{
	return curr;
}

const vector<Particle>& ParticlesContainer::getParticles() const
{
	return particleBuff;
}

int ParticlesContainer::getParticlesCount() const
{
	return particleCount;
}