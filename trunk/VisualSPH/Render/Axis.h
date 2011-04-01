#pragma once
#include "gobject.h"
class Axis : public GObject
{
private:
	struct CONSTANT_BUFFER
	{
		D3DXMATRIX mWorldViewProj;   
	};
	struct Vertex
	{
		D3DXVECTOR4 pos;
		D3DXCOLOR	col;
	};

	ID3D10VertexShader*			pVertexShader;
	ID3D10InputLayout*			pVertexLayout;
	ID3D10PixelShader*			pPixelShader;
	ID3D10DepthStencilState*	pDepthStencilState;
	ID3D10RasterizerState*		pRasterizerState;
	ID3D10BlendState*			pBlendState;
	ID3D10Buffer*	mCB;

public:
	Axis(void);
	~Axis(void);
	void init(ID3D10Device* device);
	void draw();
	void onFrameMove(D3DXMATRIX& mWorldViewProj);

};

