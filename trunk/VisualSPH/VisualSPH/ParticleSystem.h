#pragma once
#include "Particle.h"
class ParticleSystem
{
public:
	ParticleSystem(void);
	virtual Particle* getFrame(int num) = 0;
	~ParticleSystem(void);	
};

