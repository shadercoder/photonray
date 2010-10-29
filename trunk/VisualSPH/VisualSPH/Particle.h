#pragma once
#include <d3d9.h>
#include <d3dx9.h>
#include <d3dx9core.h>
#include "Vertexes.h"
#include "Vector.h"

// "particles" which containt in vertex buffer. Like as "View"
class ParticleDX
{
public:
	ParticleVertex position;
	VectorDX vector;
};

// Particle containt all information about particle. Like as "Model"
struct Particle 
{
	D3DXVECTOR3 position;
	D3DXVECTOR3 velocity;
	FLOAT density;
	FLOAT pressure;
	FLOAT mass;
	FLOAT viscosity;
	ParticleDX presentation;
};
