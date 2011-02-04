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
	getFrame(first);
}

ParticlesContainer::ParticlesContainer(void)
{
}


ParticlesContainer::~ParticlesContainer(void)
{
}

void ParticlesContainer::open(int curr)
{
	char fileName[50];
	sprintf_s(fileName, 50, "%s%05d%s", pathToFolder, curr, filePattern);
	
	//fopen_s(&pIn, fileName, "r");
	//if (NULL == pIn)
	//{
	//	return;
	//}
}
void ParticlesContainer::close()
{
	fclose(pIn);
}
Particle* ParticlesContainer::getFrame(int num)
{
//	open(num);
	// Пропустить первую строчку
	//char junk[128];
	char fileName[64] = {};
	sprintf_s(fileName, 64, "%s%05d%s", pathToFolder, curr, filePattern);
	ifstream fin(fileName);
	if (fin.fail())
	{
		return NULL;
	}

	string junk;
	for(int i = 0; i < DIMENSIONS; ++i)
	{
//		fscanf_s(pIn, "%s", junk);
		fin >> junk;
	}
	// все в буффер 
	int sz = 0;
	//for(;fscanf(pIn, "%f %f %f %f %f %f %f %f", &data[sz][0], &data[sz][1], &data[sz][2], &data[sz][3], &data[sz][4], &data[sz][5], &data[sz][6], &data[sz][7]) == DIMENSIONS; ++sz);
	//particleCount = sz;
	float x,y,z, tmp;
	//for(;fscanf_s(pIn, "%f %f %f %f %f %f %f %f", &x, &y, &z, &tmp, &tmp, &tmp, &tmp, &tmp) == DIMENSIONS; )
	while(!fin.eof())
	{
		fin >> x >> y >> z >> tmp >> tmp >> tmp >> tmp >> tmp;
		particleBuff[sz++] = Particle(x * 128 , y * 128, z * 128, 0,0,0,0,0);
	}
	particleCount = sz;

	//for (int i = 0; i < particleCount; ++i)
	//{
	//	particleBuff[i] = Particle(data[i][0], data[i][1], data[i][2], data[i][3], data[i][4], data[i][5], data[i][6], data[i][7]);
	//}
	//close();
	fin.close();
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

const Particle* ParticlesContainer::getParticles() const
{
	return (&particleBuff[0]);
}

int ParticlesContainer::getParticlesCount() const
{
	return particleCount;
}