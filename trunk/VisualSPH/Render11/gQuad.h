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

	CComPtr<ID3D11VertexShader>			pVertexShader;
	CComPtr<ID3D11InputLayout>			pVertexLayout;
	CComPtr<ID3D11PixelShader>			pPixelShader;
	CComPtr<ID3D11SamplerState>			pSamplerState;
	CComPtr<ID3D11BlendState>			pBlendState;
	CComPtr<ID3D11RasterizerState>		pRasterizerState;
	CComPtr<ID3D11DepthStencilState>	pDepthStencilState;
	

	CComPtr<ID3D11Buffer>				mCB;
	CComPtr<ID3D11Buffer>				mCB_Immute;
	CComPtr<ID3D11ShaderResourceView>	frontSRV;
	CComPtr<ID3D11ShaderResourceView>	backSRV;
	CComPtr<ID3D11ShaderResourceView>	volume;
	CComPtr<ID3D11ShaderResourceView>	pNoiseSRV;
	CComPtr<ID3D11ShaderResourceView>	pBackgroundSRV;
public:
	gQuad(void);
	~gQuad(void);
	void setVolume(CComPtr<ID3D11ShaderResourceView> _vol);
	void setNoise(CComPtr<ID3D11ShaderResourceView> _pNoiseSRV);
	void setBackground(CComPtr<ID3D11ShaderResourceView> _pBackgroundSRV);
	void draw();
	void onFrameMove(D3DXMATRIX& mWorldViewProj, CComPtr<ID3D11ShaderResourceView> frontSRV, CComPtr<ID3D11ShaderResourceView> backSRV);
	HRESULT init(CComPtr<ID3D11Device> device, CComPtr<ID3D11DeviceContext> md3dContext);
};
