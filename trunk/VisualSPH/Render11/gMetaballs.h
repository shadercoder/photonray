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
#include "sorterGPU.h"

using namespace tbb;

using namespace std;

class CalcField
{
private:


	sorterGPU sorter;
	DenseField* field;
	const Particle* particles;		
	float metaballsSize;
	float scale;
	const float threadshold;
	inline static float calcMetaball(const D3DXVECTOR3& centerBall, const D3DXVECTOR3& cell, const float threadshold)
	{	
		D3DXVECTOR3 tmp = centerBall - cell;	
		//float len = pow(tmp.x, 2) + pow(tmp.y, 2) + pow(tmp.z, 2);
		float len = D3DXVec3Dot(&tmp, &tmp);
		if (len > threadshold) {
			return 0.0f;
		}
		float res = powf(threadshold / (len + 1e-5f), 4.0f);
		return res;
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
		float* fieldData = (float*) field->getData();
		for (size_t i = r.begin(); i != r.end(); ++i)
		{
			int x = (int) floor(particles[i].position.x * scale);
			int y = (int) floor(particles[i].position.y * scale);
			int z = (int) floor(particles[i].position.z * scale);
			for (int dz = (int) -metaballsSize; dz <= (int) metaballsSize; ++dz)
			{
				for (int dx = (int) -metaballsSize; dx <= (int) metaballsSize; ++dx)
				{
					for (int dy = (int) -metaballsSize; dy <= (int) metaballsSize; ++dy)
					{
						D3DXVECTOR3 cell((float) (x + dx), (float) (y + dy), (float) (z + dz));
						if(field->isInside(x + dx, y + dy, z + dz))
						{
							//field->lvalue(x + dx, y + dy, z + dz) += calcMetaball(particles[i].position * scale, cell, threadshold);
							fieldData[field->arrayIndexFromCoordinate(x + dx, y + dy, z + dz)] += calcMetaball(particles[i].position * scale, cell, threadshold);
						}
					}
				}
			}			
		}
	}
};



class gMetaballs :	public GObject
{
private:	
	struct VS_CONSTANT_BUFFER
	{
	    D3DXMATRIX mWorldViewProj;      //mWorldViewProj will probably be global to all shaders in a project.
	};

	struct Item
	{
		UINT key;
		UINT val;
	};

	struct UINT2
	{
		UINT x;
		UINT y;
	};
	
	struct SortCB
	{
		UINT iLevel;
		UINT iLevelMask;
		UINT iWidth;
		UINT iHeight;
	};

	ID3D11VertexShader*			pVertexShader;
	ID3D11InputLayout*			pVertexLayout;
	ID3D11PixelShader*			pPixelShader;
	
	ID3D11Buffer*				mCB;
	ID3D11Buffer*				g_pSortCB;
	ID3D11BlendState*			pBlendState;
	ID3D11RasterizerState*		pRasterizerStateBack;
	ID3D11RasterizerState*		pRasterizerStateFront;
	ID3D11DepthStencilState*	pDepthStencilState;

	ID3D11Texture2D*			pFrontS;
	ID3D11RenderTargetView*		pFrontSView;
	ID3D11Texture2D*			pBackS;
	ID3D11RenderTargetView*		pBackSView;
	ID3D11ShaderResourceView*	pFrontSRV;
	ID3D11ShaderResourceView*	pBackSRV;

	ID3D11Texture3D*			pVolume;
	ID3D11ShaderResourceView*	volumeSRV;

	ID3D11Texture2D*			pNoise;
	ID3D11ShaderResourceView*	pNoiseSRV;

	ID3D11RenderTargetView*		pRenderTargetView;
	ID3D11Texture2D*			pDepthStencilBuffer;
	ID3D11DepthStencilView*		pDepthStencilView;

	ID3D11ComputeShader*		p_MetaballsProcess;
	ID3D11ComputeShader*		p_CSbuildGrid;
	ID3D11ComputeShader*		g_pBuildGridIndicesCS;
	ID3D11ComputeShader*		g_pRearrangeParticlesCS;
	ID3D11ComputeShader*		g_pSortBitonic;
	ID3D11ComputeShader*		g_pSortTranspose;
	ID3D11ComputeShader*		g_pBuildGridCS;
	
	ID3D11Buffer*				p_Volume;
	ID3D11ShaderResourceView*	p_VolumeSRV;
	ID3D11UnorderedAccessView*	p_VolumeUAV;

	ID3D11Buffer*				p_Particles;
	ID3D11ShaderResourceView*	p_ParticlesSRV;
	ID3D11UnorderedAccessView*	p_ParticlesUAV;
	
	ID3D11Buffer*				p_Grid;
	ID3D11ShaderResourceView*	p_GridSRV;
	ID3D11UnorderedAccessView*	p_GridUAV;

	ID3D11Buffer*				p_Index;
	ID3D11ShaderResourceView*	p_IndexSRV;
	ID3D11UnorderedAccessView*	p_IndexUAV;

	ID3D11Buffer*				g_pGridPingPong;
	ID3D11ShaderResourceView*	g_pGridPingPongSRV;
	ID3D11UnorderedAccessView*	g_pGridPingPongUAV;

	UINT screenWidth;
	UINT screenHeight;
	UINT volumeResolution;

	sorterGPU sorter;
	gQuad quad;
	DenseField field;
	float scale;
	float metaballsSize;

	HRESULT onCreate();
	HRESULT createTexture2D();
	HRESULT createTexture3D();

	HRESULT CreateVolumeBuffer();
	HRESULT CreateParticleBuffer(const Particle* p, int particleCount);
	HRESULT CreateGridBuffer();
	HRESULT CreateIndexBuffer();
	HRESULT CreateTempBuffer();

	void GPUSort(ID3D11DeviceContext* pd3dImmediateContext,
             ID3D11UnorderedAccessView* inUAV, ID3D11ShaderResourceView* inSRV,
             ID3D11UnorderedAccessView* tempUAV, ID3D11ShaderResourceView* tempSRV,
			 UINT numElements);


	void drawBox();
	float calcMetaball(D3DXVECTOR3 centerBall, D3DXVECTOR3 cell);
public:	
	UINT mNumMetaballs;
	void updateVolume(const vector<Particle>& particles, int numParticles, float scale, float metaballsSize);
	void updateVolumeGPU(const vector<Particle>& particles, int numParticles, float scale, float metaballsSize);
	void draw();
	void onFrameMove(D3DXMATRIX& mWorldViewProj);
	void onFrameResize(int width, int height);
	void init(ID3D11Device* device, ID3D11DeviceContext* md3dContext, int _screenWidth, int _screenHeight, int _volumeResolution);
	gMetaballs(void);
	~gMetaballs(void);
};

