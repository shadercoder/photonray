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

	ID3D11VertexShader*			pVertexShader;
	ID3D11InputLayout*			pVertexLayout;
	ID3D11PixelShader*			pPixelShader;
	ID3D11DepthStencilState*	pDepthStencilState;
	ID3D11RasterizerState*		pRasterizerState;
	ID3D11BlendState*			pBlendState;
	ID3D11Buffer*	mCB;

public:
	Axis(void);
	~Axis(void);
	void init(ID3D11Device* device, ID3D11DeviceContext* md3dContext);
	void draw();
	void onFrameMove(D3DXMATRIX& mWorldViewProj);

};

