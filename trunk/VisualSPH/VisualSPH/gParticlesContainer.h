#pragma once
#include "Particle.h"
#include "Common\GObject.h"

class gParticlesContainer: public GObject
{
private:
	const int MAX_PARTICLES;
	UINT mNumParticles;
public:
	gParticlesContainer(void);
	~gParticlesContainer(void);
	void init(ID3D10Device* device);
	void draw();
	void loadData( Particle* p, int particleCount);
};

