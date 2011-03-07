#pragma once
#include <vector>
#include "GObject.h"
#include "gQuad.h"
#include "DenseField.h"
#include "Particle.h"
#include <algorithm>
#include "tbb/tbb.h"
#include "tbb/parallel_for.h"
#include "tbb/blocked_range.h"

using namespace tbb;

using namespace std;

class CalcField
{
private:
	DenseField* field;
	const Particle* particles;		
	float metaballsSize;
	float scale;
	const float threadshold;
	inline static float calcMetaball(const D3DXVECTOR3 centerBall, const D3DXVECTOR3 cell, const float threadshold)
	{	
		D3DXVECTOR3 tmp = centerBall - cell;	
		//float len = pow(tmp.x, 2) + pow(tmp.y, 2) + pow(tmp.z, 2);
		float len = D3DXVec3Dot(&tmp, &tmp);
		if (len > threadshold) {
			return 0.0f;
		}
		return 1.0f / (len + 1e-5f);
	}
public:	
	CalcField(DenseField* field, const Particle* particles, float metaballsSize, float scale)
		:threadshold(metaballsSize * metaballsSize)
	{
		this->field = field;
		this->particles = particles;		
		this->metaballsSize = metaballsSize;
		this->scale = scale;		
	}
	
	void operator()(const blocked_range<size_t>& r) const
	{
		for (size_t i = r.begin(); i != r.end(); ++i)
		{
			int x = (int) (particles[i].position.x * scale);
			int y = (int) (particles[i].position.y * scale);
			int z = (int) (particles[i].position.z * scale);
			for (int dz = (int) -metaballsSize; dz <= (int) metaballsSize; ++dz)
			{
				for (int dx = (int) -metaballsSize; dx <= (int) metaballsSize; ++dx)
				{
					for (int dy = (int) -metaballsSize; dy <= (int) metaballsSize; ++dy)
					{
						D3DXVECTOR3 cell((float) (x + dx), (float) (y + dy), (float) (z + dz));
						if(field->isInside(x + dx, y + dy, z + dz))
						{
							field->lvalue(x + dx, y + dy, z + dz) += calcMetaball(particles[i].position * scale, cell, threadshold);
						}
					}
				}
			}			
		}
	}
};


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

