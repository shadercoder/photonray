#pragma once
#include "d3dUtil.h"

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

	CComPtr<ID3D11Device> md3dDevice;
	CComPtr<ID3D11DeviceContext> md3dContext;
	CComPtr<ID3D11Buffer> mVB;
	CComPtr<ID3D11Buffer> mIB;
	D3D11_PRIMITIVE_TOPOLOGY primitiveTopology;
	// render's parameters are set in this method before render
	void onRenderBegin();
	// restore previous render setup
	void onRenderEnd();
};

