#pragma once
#include "d3dUtil.h"
#include <vector>
#include <algorithm>

// The number of elements to sort is limited to an even power of 2
// At minimum 8,192 elements - BITONIC_BLOCK_SIZE * TRANSPOSE_BLOCK_SIZE
// At maximum 262,144 elements - BITONIC_BLOCK_SIZE * BITONIC_BLOCK_SIZE
const UINT NUM_ELEMENTS = 512 * 512;
const UINT BITONIC_BLOCK_SIZE = 512;
const UINT TRANSPOSE_BLOCK_SIZE = 16;
const UINT MATRIX_WIDTH = BITONIC_BLOCK_SIZE;
const UINT MATRIX_HEIGHT = NUM_ELEMENTS / BITONIC_BLOCK_SIZE;

class sorterGPU
{
private:
	//std::vector<UINT> data;
	void* data;
	//std::vector<UINT> results;
	void* result;

	ID3D11Device*               g_pd3dDevice;
	ID3D11DeviceContext*        g_pd3dImmediateContext;
	ID3D11ComputeShader*        g_pComputeShaderBitonic;
	ID3D11ComputeShader*        g_pComputeShaderTranspose;
	ID3D11Buffer*               g_pCB;
	ID3D11Buffer*               g_pBuffer1;
	ID3D11ShaderResourceView*   g_pBuffer1SRV;
	ID3D11UnorderedAccessView*  g_pBuffer1UAV;
	ID3D11Buffer*               g_pBuffer2;
	ID3D11ShaderResourceView*   g_pBuffer2SRV;
	ID3D11UnorderedAccessView*  g_pBuffer2UAV;
	ID3D11Buffer*               g_pReadBackBuffer;
	
	// Constant Buffer Layout
	struct CB
	{
		UINT iLevel;
		UINT iLevelMask;
		UINT iWidth;
		UINT iHeight;
	};


	HRESULT CreateResources();
	void SetConstants( UINT iLevel, UINT iLevelMask, UINT iWidth, UINT iHeight );
	

public:
	HRESULT updateBuffer(void* newData);
	void GPUSort();
	sorterGPU(void);
	~sorterGPU(void);
};

