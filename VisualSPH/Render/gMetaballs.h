#pragma once
#include "GObject.h"
#include "gQuad.h"
#include "DenseField.h"
#include "Particle.h"
#define THRESHOLD 5.0f

struct VS_CONSTANT_BUFFER
{
    D3DXMATRIX mWorldViewProj;      //mWorldViewProj will probably be global to all shaders in a project.
};



class gMetaballs :	public GObject
{
private:
	ID3D10VertexShader*			pVertexShader;
	ID3D10InputLayout*			pVertexLayout;
	ID3D10PixelShader*			pPixelShader;
	
	ID3D10Buffer*				mCB;
	ID3D10BlendState*			pBlendState;
	ID3D10RasterizerState*		pRasterizerState;
	ID3D10DepthStencilState*	pDepthStencilState;
	ID3D10Blob*					pBlob;

	ID3D10Texture2D*			pFrontS;
	ID3D10RenderTargetView*		pFrontSView;
	ID3D10Texture2D*			pBackS;
	ID3D10RenderTargetView*		pBackSView;
	ID3D10ShaderResourceView*	pFrontSRV;
	ID3D10ShaderResourceView*	pBackSRV;

	ID3D10Texture3D*			pVolume;
	ID3D10ShaderResourceView*	volumeSRV;
	ID3D10RenderTargetView*		pRenderTargetView;
	ID3D10Texture2D*			pDepthStencilBuffer;
	ID3D10DepthStencilView*		pDepthStencilView;

	ID3D10Buffer*				pVBQuad;
	ID3D10Buffer*				pIBQuad;


	UINT screenWidth;
	UINT screenHeight;
	UINT volumeResolution;

	gQuad quad;
	DenseField field;

	HRESULT onCreate();
	HRESULT createTexture();

	void drawBox();
	static float calcMetaball(D3DXVECTOR3 centerBall, D3DXVECTOR3 cell);
public:
	UINT mNumMetaballs;
	void updateVolume(const Particle* particles, int numParticles);
	void draw();
	void onFrameMove(D3DXMATRIX& mWorldViewProj);
	void init(ID3D10Device* device, int _screenWidth, int _screenHeight, int _volumeResolution);
	gMetaballs(void);
	~gMetaballs(void);
};

