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

	ID3D11VertexShader*			pVertexShader;
	ID3D11PixelShader*			pPixelShader;
	ID3D11GeometryShader*		pGeometryShader;
	ID3D11InputLayout*			pVertexLayout;
	ID3D11SamplerState*			pSamplerState;
	ID3D11BlendState*			pBlendState;
	ID3D11RasterizerState*		pRasterizerState;
	ID3D11DepthStencilState*	pDepthStencilState;
	ID3DBlob*					pBlob;
	ID3D11Buffer*				mCB;
	ID3D11Buffer*				mCBImmute;
	ID3D11ShaderResourceView*	pTextSRV;

public:
	gLabel(void);
	~gLabel(void);
	void setPosition(float x, float y, float z, float height, float width);
	void init(ID3D11Device* device, ID3D11DeviceContext* md3dContext, LPCWSTR textFileName);
	void draw();
	void onFrameMove(D3DXMATRIX WorldViewProj, D3DXMATRIX View);
};

