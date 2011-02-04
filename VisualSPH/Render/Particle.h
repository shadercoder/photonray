#pragma once
#include "d3dUtil.h"


// Particle containt all information about particle. Like as "Model"
struct Particle 
{
	D3DXVECTOR3 position;
	D3DXVECTOR3 velocity;
	FLOAT density;
	FLOAT pressure;

	Particle() {}
	Particle(float x, float y, float z, float vx, float vy, float vz, float _density,
		float _pressure) {
			position = D3DXVECTOR3(x, y, z);
			velocity = D3DXVECTOR3(vx, vy, vz);
			density = _density;
			pressure = _pressure;
	}
};
