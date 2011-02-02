#pragma once
#include "d3dUtil.h"
#include "Vertex.h"

class GObject
{
public:
	GObject(void);
	virtual ~GObject(void);

	virtual void draw() = 0;

protected:
	DWORD mNumVertices;
	DWORD mNumIndices;
	DWORD mNumFaces;	

	ID3D10Device* md3dDevice;
	ID3D10Buffer* mVB;
	ID3D10Buffer* mIB;
	D3D10_PRIMITIVE_TOPOLOGY primitiveTopology;
	// render's parameters are set in this method before render
	void onRenderBegin();
	// restore previous render setup
	void onRenderEnd();
};

