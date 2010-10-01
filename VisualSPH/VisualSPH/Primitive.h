#pragma once
#include <d3d9.h>
#include <d3dx9.h>
// Base class for all primitives (vector,point,sphere...)
class Primitive
{
public:
	D3DXVECTOR3 position;

	Primitive(void);
	~Primitive(void);
};

