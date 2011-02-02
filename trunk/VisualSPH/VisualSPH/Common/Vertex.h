#pragma once
#include "common\d3dUtil.h"

struct Vertex
{
	D3DXVECTOR3 pos;
	D3DXVECTOR3 nor;
	D3DXCOLOR   color;
};

struct Vertex2D
{
	D3DXVECTOR3 pos;
	D3DXVECTOR2 tcoord;
	D3DXCOLOR   color;
};

struct VertexParticle
{
	D3DXVECTOR3 pos;
	D3DXVECTOR2 size;
};
