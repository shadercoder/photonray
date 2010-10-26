#pragma once
#include <Windows.h>

struct Vertex
{
	FLOAT x,y,z;
	DWORD color;
	float	tx,ty;
};
// TODO: texture coordinates + normal
struct ParticleVertex: public Vertex
{

};
#define D3DFVF_Vertex (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1)
#define D3DFVF_PARTICLE (D3DFVF_XYZ | D3DFVF_DIFFUSE)
