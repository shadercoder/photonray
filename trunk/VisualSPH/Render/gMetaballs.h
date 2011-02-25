#pragma once
#include <vector>
#include "GObject.h"
#include "gQuad.h"
#include "DenseField.h"
#include "Particle.h"

using namespace std;

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
	ID3D10RasterizerState*		pRasterizerStateBack;
	ID3D10RasterizerState*		pRasterizerStateFront;
	ID3D10DepthStencilState*	pDepthStencilState;

	ID3D10Texture2D*			pFrontS;
	ID3D10RenderTargetView*		pFrontSView;
	ID3D10Texture2D*			pBackS;
	ID3D10RenderTargetView*		pBackSView;
	ID3D10ShaderResourceView*	pFrontSRV;
	ID3D10ShaderResourceView*	pBackSRV;

	ID3D10Texture3D*			pVolume;
	ID3D10ShaderResourceView*	volumeSRV;

	ID3D10Texture2D*			pNoise;
	ID3D10ShaderResourceView*	pNoiseSRV;

	ID3D10RenderTargetView*		pRenderTargetView;
	ID3D10Texture2D*			pDepthStencilBuffer;
	ID3D10DepthStencilView*		pDepthStencilView;

	UINT screenWidth;
	UINT screenHeight;
	UINT volumeResolution;

	gQuad quad;
	DenseField field;
	float scale;
	float metaballsSize;

	HRESULT onCreate();
	HRESULT createTexture2D();
	HRESULT createTexture3D();

	void drawBox();
	float calcMetaball(D3DXVECTOR3 centerBall, D3DXVECTOR3 cell);
public:
	UINT mNumMetaballs;
	void updateVolume(const vector<Particle>& particles, int numParticles, float scale, float metaballsSize);
	void draw();
	void onFrameMove(D3DXMATRIX& mWorldViewProj);
	void onFrameResize(int width, int height);
	void init(ID3D10Device* device, int _screenWidth, int _screenHeight, int _volumeResolution);
	gMetaballs(void);
	~gMetaballs(void);
};
