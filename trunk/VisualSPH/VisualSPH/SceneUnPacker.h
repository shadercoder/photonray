#include "SceneConstraints.h"
#include <cstdio>

class SceneUnPacker
{
	int frames;
	int curr_frame;
	FILE* pIn;
public:
	float buff[DIMENSIONS];
	SceneUnPacker(char fn[]);
	float particles[MAX_PARTICLES][DIMENSIONS];
	bool getNextParticles();
};
