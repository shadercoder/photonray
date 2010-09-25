#pragma once
#include <d3d9.h>
#include <d3dx9.h>
#include <d3dx9core.h>
#include "Vertexes.h"

class Particle
{
public:
	CUSTOMVERTEX pos;
	CUSTOMVERTEX x0; // start point vector of velocity
	CUSTOMVERTEX x1; // end point vector of velocity
};

