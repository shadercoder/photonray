#pragma once
#include <d3d9.h>
#include <d3dx9.h>
#include <d3dx9core.h>

class Particle
{
public:
	D3DXVECTOR3 position;	
	D3DCOLOR color;
};

struct ParticleAttribute
{
     D3DXVECTOR3 position;
	 D3DXVECTOR3 velocity;
	 float Ro;
	 float P;
};

