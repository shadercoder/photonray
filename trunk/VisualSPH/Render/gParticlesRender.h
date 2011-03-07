#pragma once
#include <vector>
#include "gobject.h"
#include "Particle.h"
#include "For_Color.h"

using namespace std;

class gParticlesRender : public GObject
{
private:

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


	ID3D10VertexShader*			pVertexShader;
	ID3D10PixelShader*			pPixelShader;
	ID3D10GeometryShader*		pGeometryShader;
	ID3D10InputLayout*			pVertexLayout;
	ID3D10SamplerState*			pSamplerState;
	ID3D10BlendState*			pBlendState;
	ID3D10RasterizerState*		pRasterizerState;
	ID3D10DepthStencilState*	pDepthStencilState;
	ID3D10Blob*					pBlob;
	ID3D10Buffer*				mCB;
	ID3D10Buffer*				mCBImmute;
	ID3D10ShaderResourceView*	pTexParticleSRV;
	For_color					painter;
public:
	gParticlesRender(void);
	~gParticlesRender(void);

	void updateParticles(const vector<Particle>& particles);
	void onFrameMove(D3DXMATRIX WorldViewProj, D3DXMATRIX View);
	void draw();
	HRESULT init(ID3D10Device* device);
};

