#pragma once
#include <Windows.h>

struct CUSTOMVERTEX
{
	FLOAT x,y,z;
	DWORD color;
};
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE)
#define D3DFVF_PARTICLE (D3DFVF_XYZ | D3DFVF_DIFFUSE)
