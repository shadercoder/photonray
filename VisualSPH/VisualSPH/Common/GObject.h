#pragma once
#include "d3dUtil.h"
#include "Vertex.h"

class GObject
{
public:
	GObject(void);
	virtual ~GObject(void);

//	virtual void init(ID3D10Device* device, D3D10_PRIMITIVE_TOPOLOGY topology, ...) = 0;
	virtual void draw() = 0;

protected:
	DWORD mNumVertices;
	DWORD mNumFaces;

	ID3D10Device* md3dDevice;
	ID3D10Buffer* mVB;
	ID3D10Buffer* mIB;
	D3D10_PRIMITIVE_TOPOLOGY primitiveTopology;
};

