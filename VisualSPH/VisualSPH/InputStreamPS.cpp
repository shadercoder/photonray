#include "InputStreamPS.h"


void InputStreamPS::init(ID3D10Device* device, string& _pathToFolder, string& _filePattern, int _first, int _last, int _step)
{
	this->device = device;
	pathToFolder = _pathToFolder.c_str();
	filePattern = _filePattern.c_str();
	first = _first;
	last = _last;
	step = _step;

	curr = first;
	// TODO : remove magic constants
	particleView.init(device, 800, 600, 64);
}

InputStreamPS::InputStreamPS(void)
{
}


InputStreamPS::~InputStreamPS(void)
{
}

void InputStreamPS::open(int curr)
{
	char fileName[50];
	sprintf(fileName, "%s%05d%s", pathToFolder, curr, filePattern);
	pIn = fopen(fileName, "rb");
}
void InputStreamPS::close()
{
	fclose(pIn);
}
Particle* InputStreamPS::getFrame(int num)
{
	open(num);
	// Пропустить первую строчку
	char junk[32];
	for(int i = 0; i < DIMENSIONS; ++i)
		fscanf(pIn, "%s", junk);
	// все в буффер 
	int sz = 0;
	for(;fscanf(pIn, "%f %f %f %f %f %f %f %f", &data[sz][0], &data[sz][1], &data[sz][2], &data[sz][3], &data[sz][4], &data[sz][5], &data[sz][6], &data[sz][7]) == DIMENSIONS; ++sz);
	particleCount = sz;

	for (int i = 0; i < particleCount; ++i)
	{
		particleBuff[i] = Particle(data[i][0], data[i][1], data[i][2], data[i][3], data[i][4], data[i][5], data[i][6], data[i][7]);
	}
	close();
	return (&particleBuff[0]);
}

Particle* InputStreamPS::getNextFrame()
{
	if ((curr + step) > last)
	{
		curr = first;
	} 
	else
	{
		curr += step;
	}
	return (getFrame(curr));
}

void InputStreamPS::drawAll()
{
	// draw all particles
	//particleView.init(device, &particleBuff[0], particleCount);

	//particleView.loadData(&particleBuff[0], particleCount);
	//particleView.draw();
	//device->RSSetState(0); // restore default
	particleView.loadData(&particleBuff[0], particleCount);
	//particleView.update();
	particleView.draw();
}

int InputStreamPS::getNumCurrFrame()
{
	return curr;
}
