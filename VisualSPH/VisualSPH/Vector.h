#pragma once
#include "Primitive.h"
#include "Vertexes.h"

class Vector: public Primitive
{
public:
	D3DXVECTOR3 vector;
	Vector(float x, float y, float z);
	Vector(void);
	~Vector(void);
};

class VectorDX
{
public:
	Vertex begin;
	Vertex end;
};
