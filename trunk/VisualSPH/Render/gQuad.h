#pragma once
#include "GObject.h"
#include "Vertex.h"



class gQuad :
	public GObject
{
private:
	struct CONSTANT_BUFFER
	{
		D3DXMATRIX mWorldViewProj;   
		D3DXVECTOR4 vLightPos1;
		D3DXVECTOR4 vLightPos2;
		D3DXVECTOR4 vMaterial;
	};

	ID3D10VertexShader*			pVertexShader;
	ID3D10InputLayout*			pVertexLayout;
	ID3D10PixelShader*			pPixelShader;
	ID3D10SamplerState*			pSamplerState;
	ID3D10BlendState*			pBlendState;
	ID3D10RasterizerState*		pRasterizerState;
	ID3D10DepthStencilState*	pDepthStencilState;
	ID3D10Blob*					pBlob;

	ID3D10Buffer*				mCB;
	ID3D10ShaderResourceView*	frontSRV;
	ID3D10ShaderResourceView*	backSRV;
	ID3D10ShaderResourceView*	volume;
	ID3D10ShaderResourceView*	pNoiseSRV;
public:
	gQuad(void);
	~gQuad(void);
	void setVolume(ID3D10ShaderResourceView* _vol);
	void setNoise(ID3D10ShaderResourceView* _pNoiseSRV);
	void draw();
	void onFrameMove(D3DXMATRIX& mWorldViewProj, ID3D10ShaderResourceView* frontSRV, ID3D10ShaderResourceView* backSRV);
	HRESULT init(ID3D10Device* device);//, ID3D10ShaderResourceView* frontS, ID3D10ShaderResourceView* backS);
};
