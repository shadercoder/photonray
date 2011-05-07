#pragma once
#include "gobject.h"
#include "gLabel.h"

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

	CComPtr<ID3D11VertexShader>			pVertexShader;
	CComPtr<ID3D11InputLayout>			pVertexLayout;
	CComPtr<ID3D11PixelShader>			pPixelShader;
	CComPtr<ID3D11DepthStencilState>	pDepthStencilState;
	CComPtr<ID3D11RasterizerState>		pRasterizerState;
	CComPtr<ID3D11BlendState>			pBlendState;
	CComPtr<ID3D11Buffer>	mCB;
	gLabel xLabel, yLabel, zLabel;
public:
	Axis(void);
	virtual ~Axis(void);
	void init(CComPtr<ID3D11Device> device, CComPtr<ID3D11DeviceContext> md3dContext);
	void draw();
	void onFrameMove(D3DXMATRIX& mWorldViewProj, D3DXMATRIX View);

};

