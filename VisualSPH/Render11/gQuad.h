#pragma once
#include "GObject.h"
#include "Vertex.h"
//#include "ShaderUtils.h"

class gQuad :
	public GObject
{
private:
	struct CONSTANT_BUFFER
	{
		D3DXMATRIX mWorldViewProj;   
	};
	struct Light
	{
		D3DXVECTOR4 pos;
		D3DXCOLOR color;
	};
	struct Material
	{
		float Ka, Kd, Ks, A;
	};
	struct CBUFFER_IMMUTE
	{
		D3DXCOLOR ambientLight;
		Light l1, l2;
		Material material;
	};

	ID3D11VertexShader*			pVertexShader;
	ID3D11InputLayout*			pVertexLayout;
	ID3D11PixelShader*			pPixelShader;
	ID3D11SamplerState*			pSamplerState;
	ID3D11BlendState*			pBlendState;
	ID3D11RasterizerState*		pRasterizerState;
	ID3D11DepthStencilState*	pDepthStencilState;
	

	ID3D11Buffer*				mCB;
	ID3D11Buffer*				mCB_Immute;
	ID3D11ShaderResourceView*	frontSRV;
	ID3D11ShaderResourceView*	backSRV;
	ID3D11ShaderResourceView*	volume;
	ID3D11ShaderResourceView*	pNoiseSRV;
public:
	gQuad(void);
	~gQuad(void);
	void setVolume(ID3D11ShaderResourceView* _vol);
	void setNoise(ID3D11ShaderResourceView* _pNoiseSRV);
	void draw();
	void onFrameMove(D3DXMATRIX& mWorldViewProj, ID3D11ShaderResourceView* frontSRV, ID3D11ShaderResourceView* backSRV);
	HRESULT init(ID3D11Device* device, ID3D11DeviceContext* md3dContext);
};
