#pragma once
#include "GObject.h"

class gLabel: public GObject
{
	struct CONSTANT_BUFFER
	{
		D3DXMATRIXA16 mWorldViewProj;   
		D3DXMATRIXA16 mInverseView;   
	};

	struct CONSTANT_BUFFER_IMMUTE
	{
		D3DXVECTOR4 g_positions[4];
		D3DXVECTOR4 g_texcoords[4]; 		
	};

	struct Vertex
	{
		D3DXVECTOR4 pos;	
		float	width;
		float	height;
	};

	CComPtr<ID3D11VertexShader>			pVertexShader;
	CComPtr<ID3D11PixelShader>			pPixelShader;
	CComPtr<ID3D11GeometryShader>		pGeometryShader;
	CComPtr<ID3D11InputLayout>			pVertexLayout;
	CComPtr<ID3D11SamplerState>			pSamplerState;
	CComPtr<ID3D11BlendState>			pBlendState;
	CComPtr<ID3D11RasterizerState>		pRasterizerState;
	CComPtr<ID3D11DepthStencilState>	pDepthStencilState;
	CComPtr<ID3D11Buffer>				mCB;
	CComPtr<ID3D11Buffer>				mCBImmute;
	CComPtr<ID3D11ShaderResourceView>	pTextSRV;

public:
	gLabel(void);
	virtual ~gLabel(void);
	void setPosition(float x, float y, float z, float height, float width);
	void init(CComPtr<ID3D11Device> device, CComPtr<ID3D11DeviceContext> md3dContext, LPCWSTR textFileName);
	void draw();
	void onFrameMove(D3DXMATRIX WorldViewProj, D3DXMATRIX View);
};

