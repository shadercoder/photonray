#pragma once
#include <Windows.h>

struct Vertex
{
	FLOAT x,y,z;
	DWORD color;
};
// TODO: texture coordinates + normal
struct ParticleVertex: public Vertex
{

};
#define D3DFVF_Vertex (D3DFVF_XYZ | D3DFVF_DIFFUSE)
#define D3DFVF_PARTICLE (D3DFVF_XYZ | D3DFVF_DIFFUSE)
