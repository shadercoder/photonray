#include "DXUT.h"
#include "gMetaballs.h"


gMetaballs::gMetaballs(void)
{
}

gMetaballs::~gMetaballs(void)
{
	quad.~gQuad();
	//sorter.~sorterGPU();
}

float gMetaballs::calcMetaball(D3DXVECTOR3 centerBall, D3DXVECTOR3 cell)
{	
	return 0.0f;
}

//--------------------------------------------------------------------------------------
// Create a CPU accessible buffer and download the content of a GPU buffer into it
// This function is very useful for debugging CS programs
//-------------------------------------------------------------------------------------- 
ID3D11Buffer* CreateAndCopyToDebugBuf( ID3D11Device* pDevice, ID3D11DeviceContext* pd3dImmediateContext, ID3D11Buffer* pBuffer )
{
    ID3D11Buffer* debugbuf = NULL;

    D3D11_BUFFER_DESC desc;
    ZeroMemory( &desc, sizeof(desc) );
    pBuffer->GetDesc( &desc );
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    desc.Usage = D3D11_USAGE_STAGING;
    desc.BindFlags = 0;
    desc.MiscFlags = 0;
    if ( SUCCEEDED(pDevice->CreateBuffer(&desc, NULL, &debugbuf)) )
    {
#if defined(DEBUG) || defined(PROFILE)
        debugbuf->SetPrivateData( WKPDID_D3DDebugObjectName, sizeof( "Debug" ) - 1, "Debug" );
#endif

        pd3dImmediateContext->CopyResource( debugbuf, pBuffer );
    }

    return debugbuf;
}

//--------------------------------------------------------------------------------------
// GPU Bitonic Sort
// For more information, please see the ComputeShaderSort11 sample
//--------------------------------------------------------------------------------------
void gMetaballs::GPUSort(ID3D11DeviceContext* pd3dImmediateContext,
             ID3D11UnorderedAccessView* inUAV, ID3D11ShaderResourceView* inSRV,
             ID3D11UnorderedAccessView* tempUAV, ID3D11ShaderResourceView* tempSRV,
			 UINT numElements)
{
    pd3dImmediateContext->CSSetConstantBuffers( 0, 1, &g_pSortCB );

    const UINT NUM_ELEMENTS = numElements;
    const UINT MATRIX_WIDTH = BITONIC_BLOCK_SIZE;
    const UINT MATRIX_HEIGHT = NUM_ELEMENTS / BITONIC_BLOCK_SIZE;

    // Sort the data
    // First sort the rows for the levels <= to the block size
    for( UINT level = 2 ; level <= BITONIC_BLOCK_SIZE ; level <<= 1 )
    {
        SortCB constants = { level, level, MATRIX_HEIGHT, MATRIX_WIDTH };
        pd3dImmediateContext->UpdateSubresource( g_pSortCB, 0, NULL, &constants, 0, 0 );

        // Sort the row data
        UINT UAVInitialCounts = 0;
        pd3dImmediateContext->CSSetUnorderedAccessViews( 0, 1, &inUAV, &UAVInitialCounts );
        pd3dImmediateContext->CSSetShader( g_pSortBitonic, NULL, 0 );
        pd3dImmediateContext->Dispatch( NUM_ELEMENTS / BITONIC_BLOCK_SIZE, 1, 1 );
    }

    // Then sort the rows and columns for the levels > than the block size
    // Transpose. Sort the Columns. Transpose. Sort the Rows.
    for( UINT level = (BITONIC_BLOCK_SIZE << 1) ; level <= NUM_ELEMENTS ; level <<= 1 )
    {
        SortCB constants1 = { (level / BITONIC_BLOCK_SIZE), (level & ~NUM_ELEMENTS) / BITONIC_BLOCK_SIZE, MATRIX_WIDTH, MATRIX_HEIGHT };
        pd3dImmediateContext->UpdateSubresource( g_pSortCB, 0, NULL, &constants1, 0, 0 );

        // Transpose the data from buffer 1 into buffer 2
        ID3D11ShaderResourceView* pViewNULL = NULL;
        UINT UAVInitialCounts = 0;
        pd3dImmediateContext->CSSetShaderResources( 0, 1, &pViewNULL );
        pd3dImmediateContext->CSSetUnorderedAccessViews( 0, 1, &tempUAV, &UAVInitialCounts );
        pd3dImmediateContext->CSSetShaderResources( 0, 1, &inSRV );
        pd3dImmediateContext->CSSetShader( g_pSortTranspose, NULL, 0 );
        pd3dImmediateContext->Dispatch( MATRIX_WIDTH / TRANSPOSE_BLOCK_SIZE, MATRIX_HEIGHT / TRANSPOSE_BLOCK_SIZE, 1 );

        // Sort the transposed column data
        pd3dImmediateContext->CSSetShader( g_pSortBitonic, NULL, 0 );
        pd3dImmediateContext->Dispatch( NUM_ELEMENTS / BITONIC_BLOCK_SIZE, 1, 1 );

        SortCB constants2 = { BITONIC_BLOCK_SIZE, level, MATRIX_HEIGHT, MATRIX_WIDTH };
        pd3dImmediateContext->UpdateSubresource( g_pSortCB, 0, NULL, &constants2, 0, 0 );

        // Transpose the data from buffer 2 back into buffer 1
        pd3dImmediateContext->CSSetShaderResources( 0, 1, &pViewNULL );
        pd3dImmediateContext->CSSetUnorderedAccessViews( 0, 1, &inUAV, &UAVInitialCounts );
        pd3dImmediateContext->CSSetShaderResources( 0, 1, &tempSRV );
        pd3dImmediateContext->CSSetShader( g_pSortTranspose, NULL, 0 );
        pd3dImmediateContext->Dispatch( MATRIX_HEIGHT / TRANSPOSE_BLOCK_SIZE, MATRIX_WIDTH / TRANSPOSE_BLOCK_SIZE, 1 );

        // Sort the row data
        pd3dImmediateContext->CSSetShader( g_pSortBitonic, NULL, 0 );
        pd3dImmediateContext->Dispatch( NUM_ELEMENTS / BITONIC_BLOCK_SIZE, 1, 1 );
    }
}

void gMetaballs::updateVolumeGPU(const vector<Particle>& particles, int numParticles, float scale, float metaballsSize)
{
	//CreateVolumeBuffer();
	CreateParticleBuffer(&(particles[0]), numParticles);
	CreateGridBuffer();
	CreateIndexBuffer();
	const int BLOCK_SIZE = 256;
	UINT UAVInitialCounts = 0;
	md3dContext->CSSetUnorderedAccessViews(0, 1, &p_GridUAV, &UAVInitialCounts);
	md3dContext->CSSetShaderResources(0, 1, &p_ParticlesSRV);
	// build keys
	md3dContext->CSSetShader(g_pBuildGridCS, NULL, 0);
	md3dContext->Dispatch(numParticles, 1, 1);
	// sort keys
	GPUSort(md3dContext, p_GridUAV, p_GridSRV, g_pGridPingPongUAV, g_pGridPingPongSRV, numParticles);	
	// buil indices
	md3dContext->CSSetShader( g_pBuildGridIndicesCS, NULL, 0 );
    md3dContext->Dispatch( numParticles / BLOCK_SIZE, 1, 1 );
    // rearrange
    md3dContext->CSSetShader( g_pRearrangeParticlesCS, NULL, 0 );
    md3dContext->Dispatch( numParticles / BLOCK_SIZE, 1, 1 );


	//ID3D11Buffer* temp = CreateAndCopyToDebugBuf(md3dDevice, md3dContext, p_Volume);
	//
	//D3D11_MAPPED_SUBRESOURCE pMT;	
	//D3D11_MAPPED_SUBRESOURCE mappedResourceFromCS;	
	//HR(md3dContext->Map(pVolume, 0, D3D11_MAP_WRITE_DISCARD, 0, &pMT)); 	
	//HR(md3dContext->Map(temp, 0, D3D11_MAP_READ, 0, &mappedResourceFromCS));
	//int strideI = pMT.DepthPitch / sizeof(float);
	//int strideJ = pMT.RowPitch / sizeof(float);
	//float* textureData = (float*) pMT.pData;	
	//float* volumeData = (float*) mappedResourceFromCS.pData;
	////const float* fieldData = field.getData();
	//for (int i = 0; i < field.xSize; ++i)
	//{
	//	for (int j = 0; j < field.ySize; ++j)
	//	{
	//		//textureData[i * strideI + j * strideJ] = field.value(i, j, k);
	//		//memcpy(&textureData[i * strideI + j * strideJ], fieldData + field.arrayIndexFromCoordinate(i, j, 0), sizeof(float) * field.zSize);
	//		for (int k = 0; k < field.zSize; ++k)
	//		{
	//			//textureData[i * strideI + j * strideJ + k] = field.value(i, j, k);				
	//			textureData[i * strideI + j * strideJ + k] = volumeData[(i * volumeResolution + j) * volumeResolution + k];				
	//		}
	//		
	//	}		
	//}
	//md3dContext->Unmap(pVolume, 0);
	//md3dContext->Unmap(temp, 0);
	//SAFE_RELEASE(temp);
}

void gMetaballs::updateVolume(const vector<Particle>& particles, int numParticles, float scale, float metaballsSize)
{
	//updateVolumeGPU(particles, numParticles, scale, metaballsSize);
	
	this->scale = scale;
	this->metaballsSize = metaballsSize;
	field.clear();
	parallel_for(blocked_range<size_t>(0, numParticles), CalcField(&field, &particles[0], metaballsSize, scale), auto_partitioner());

	D3D11_MAPPED_SUBRESOURCE pMT;	
	HR(md3dContext->Map(pVolume, 0, D3D11_MAP_WRITE_DISCARD, 0, &pMT)); 	
	int strideI = pMT.DepthPitch / sizeof(float);
	int strideJ = pMT.RowPitch / sizeof(float);
	float* textureData = (float*) pMT.pData;	
	const float* fieldData = field.getData();
	for (int i = 0; i < field.xSize; ++i)
	{
		for (int j = 0; j < field.ySize; ++j)
		{
			//textureData[i * strideI + j * strideJ] = field.value(i, j, k);
			memcpy(&textureData[i * strideI + j * strideJ], fieldData + field.arrayIndexFromCoordinate(i, j, 0), sizeof(float) * field.zSize);
			for (int k = 0; k < field.zSize; ++k)
			{
				//textureData[i * strideI + j * strideJ + k] = field.value(i, j, k);				
			}
			
		}		
	}	
	md3dContext->Unmap(pVolume, 0);
	
}

void gMetaballs::init(CComPtr<ID3D11Device> device, CComPtr<ID3D11DeviceContext> md3dContext, int _screenWidth, int _screenHeight, int _volumeResolution)
{
	md3dDevice = device;
	this->md3dContext = md3dContext;
	screenHeight = _screenHeight;
	screenWidth = _screenWidth;
	volumeResolution = _volumeResolution;	
	field = DenseField(volumeResolution, volumeResolution, volumeResolution);
	onCreate();
	createTexture2D();
	createTexture3D();
	CreateVolumeBuffer();
	CreateParticleBuffer(NULL, 0);
	CreateGridBuffer();
	CreateIndexBuffer();
	CreateTempBuffer();
	quad.init(md3dDevice, md3dContext);
	quad.setNoise(pNoiseSRV);
}

void gMetaballs::onFrameResize(int width, int height)
{
	screenWidth = width;
	screenHeight = height;
	createTexture2D();
}

HRESULT gMetaballs::createTexture2D()
{
	HRESULT hr;
	// Create the render target texture
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory( &desc, sizeof(desc) );
	// TODO: set screen resolution
	desc.Width = screenWidth;
	desc.Height = screenHeight;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	hr = md3dDevice->CreateTexture2D(&desc, NULL, &pFrontS.p);
	hr = md3dDevice->CreateRenderTargetView(pFrontS, 0, &pFrontSView.p);
	hr = md3dDevice->CreateTexture2D(&desc, NULL, &pBackS.p );
	hr = md3dDevice->CreateRenderTargetView(pBackS, 0, &pBackSView.p);

	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	viewDesc.Format = desc.Format;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	viewDesc.Texture2DArray.MostDetailedMip = 0;
	viewDesc.Texture2DArray.MipLevels = desc.MipLevels;
	viewDesc.Texture2DArray.FirstArraySlice = 0;
	viewDesc.Texture2DArray.ArraySize = 0;

	md3dDevice->CreateShaderResourceView(pFrontS, 0, &pFrontSRV.p);
	md3dDevice->CreateShaderResourceView(pBackS, 0, &pBackSRV.p);

	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width     = screenWidth;
	depthStencilDesc.Height    = screenHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format    = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count   = 1; 
	depthStencilDesc.SampleDesc.Quality = 0; 
	depthStencilDesc.Usage          = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags      = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0; 
	depthStencilDesc.MiscFlags      = 0;

	hr = md3dDevice->CreateTexture2D(&depthStencilDesc, 0, &pDepthStencilBuffer.p);
	hr = md3dDevice->CreateDepthStencilView(pDepthStencilBuffer, 0, &pDepthStencilView.p);

	ZeroMemory( &desc, sizeof(desc) );
	desc.Width = 32;
	desc.Height = 32;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R32_FLOAT;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	D3D11_SUBRESOURCE_DATA noiseInit;
	noiseInit.SysMemPitch = sizeof(float) * 32;
	float* pNoiseData = new float[32 * 32];
	for (int i = 0; i < 32; ++i)
	{
		for (int j = 0; j < 32; ++j)
		{
			pNoiseData[i * 32 + j] = RandF();
		}
	}
	noiseInit.pSysMem = (void*) pNoiseData;
	HR(md3dDevice->CreateTexture2D(&desc, &noiseInit, &pNoise.p ));

	viewDesc.Format = desc.Format;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	viewDesc.Texture2DArray.MostDetailedMip = 0;
	viewDesc.Texture2DArray.MipLevels = desc.MipLevels;
	viewDesc.Texture2DArray.FirstArraySlice = 0;
	viewDesc.Texture2DArray.ArraySize = 0;

	md3dDevice->CreateShaderResourceView(pNoise, 0, &pNoiseSRV.p);
	SAFE_DELETE_ARRAY(pNoiseData);
	return hr;
}

HRESULT gMetaballs::createTexture3D()
{
	HRESULT hr = S_OK;
	D3D11_TEXTURE3D_DESC volume_desc;
	ZeroMemory(&volume_desc, sizeof(volume_desc));
	volume_desc.Width = volumeResolution;
	volume_desc.Height = volumeResolution;
	volume_desc.Depth = volumeResolution;	
	volume_desc.MipLevels = 1;
	volume_desc.Format = DXGI_FORMAT_R32_FLOAT;
	volume_desc.Usage = D3D11_USAGE_DYNAMIC; 
	volume_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	volume_desc.MiscFlags = 0;
	volume_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA densityField;
	float* fieldData = new float[volumeResolution * volumeResolution * volumeResolution];
	
	densityField.pSysMem = fieldData;
	densityField.SysMemPitch = volumeResolution * sizeof(float);
	densityField.SysMemSlicePitch = volumeResolution * volumeResolution * sizeof(float);

	D3D11_SHADER_RESOURCE_VIEW_DESC densityFieldSRVdesc;
	ZeroMemory(&densityFieldSRVdesc, sizeof(densityFieldSRVdesc));
	densityFieldSRVdesc.Format = volume_desc.Format;
	densityFieldSRVdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
	densityFieldSRVdesc.Texture3D.MipLevels = 1;
	densityFieldSRVdesc.Texture3D.MostDetailedMip = 0;

	HR(md3dDevice->CreateTexture3D(&volume_desc, &densityField, &pVolume.p));
	HR(md3dDevice->CreateShaderResourceView(pVolume, &densityFieldSRVdesc, &volumeSRV.p));

	SAFE_DELETE_ARRAY(fieldData);
	return hr;
}

HRESULT gMetaballs::onCreate()
{
	HRESULT hr;
	DWORD dwShaderFlags = 0;
#if defined( DEBUG ) || defined( _DEBUG )
	dwShaderFlags |= D3D11_SHADER_DEBUG_REG_INTERFACE_POINTERS;
#endif
	const D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	mNumVertices = 8;
	mNumFaces = 12; 
	float x_max = 1.f;  
	float x_min = 0.f;
	float y_max = 1.f;  
	float y_min = 0.f;
	float z_max = 1.f;  
	float z_min = 0.f;
	// Create vertex buffer
	Vertex vertices[] =
	{
		{D3DXVECTOR3(x_min, y_min, z_min), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f)},
		{D3DXVECTOR3(x_min, y_max, z_min), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f)},
		{D3DXVECTOR3(x_max, y_max, z_min), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f)},
		{D3DXVECTOR3(x_max, y_min, z_min), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f)},

		{D3DXVECTOR3(x_min, y_min, z_max), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f)},
		{D3DXVECTOR3(x_min, y_max, z_max), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXCOLOR(0.0f, 1.0f, 1.0f, 1.0f)},
		{D3DXVECTOR3(x_max, y_max, z_max), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f)},
		{D3DXVECTOR3(x_max, y_min, z_max), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXCOLOR(1.0f, 0.0f, 1.0f, 1.0f)},
	};

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex) * mNumVertices;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = vertices;
	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mVB));


	// Create the index buffer
	mNumIndices = 36;
	DWORD indices[] = {
		0, 1, 2,
		0, 2, 3,
		0, 4, 5,
		1, 0, 5,
		1, 5, 6,
		2, 1, 6,
		2, 6, 7,
		2, 7, 3,
		3, 7, 4,
		3, 4, 0,
		6, 5, 4,
		7, 6, 4,
	};

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(DWORD) * mNumIndices;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = indices;
	hr = md3dDevice->CreateBuffer(&ibd, &iinitData, &mIB);

	// Create a constant buffer
	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof( VS_CONSTANT_BUFFER );
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	hr = md3dDevice->CreateBuffer( &cbDesc, NULL, &mCB);


	// Create a blend state to disable alpha blending
	D3D11_BLEND_DESC BlendState;
	ZeroMemory( &BlendState, sizeof( D3D11_BLEND_DESC ) );
	BlendState.RenderTarget[0].BlendEnable = FALSE;
	BlendState.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = md3dDevice->CreateBlendState( &BlendState, &pBlendState ) ;

	// Create a rasterizer state to disable culling
	D3D11_RASTERIZER_DESC RSDesc;
	RSDesc.FillMode = D3D11_FILL_SOLID;
	RSDesc.CullMode = D3D11_CULL_FRONT;
	RSDesc.FrontCounterClockwise = TRUE;
	RSDesc.DepthBias = 0;
	RSDesc.DepthBiasClamp = 0;
	RSDesc.SlopeScaledDepthBias = 0;
	RSDesc.ScissorEnable = FALSE;
	RSDesc.MultisampleEnable = TRUE;
	RSDesc.AntialiasedLineEnable = TRUE;
	hr = md3dDevice->CreateRasterizerState( &RSDesc, &pRasterizerStateFront );
	RSDesc.CullMode = D3D11_CULL_BACK;
	hr = md3dDevice->CreateRasterizerState( &RSDesc, &pRasterizerStateBack );

	// Create a depth stencil state to enable less-equal depth testing
	D3D11_DEPTH_STENCIL_DESC DSDesc;
	ZeroMemory( &DSDesc, sizeof( D3D11_DEPTH_STENCIL_DESC ) );
	DSDesc.DepthEnable = TRUE;
	DSDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	DSDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	hr = md3dDevice->CreateDepthStencilState( &DSDesc, &pDepthStencilState );

	CComPtr<ID3DBlob> pBlob;	
	// Compile the vertex shader from the file
	CComPtr<ID3DBlob> err;
	
	hr = D3DX11CompileFromFile(L"metaballs.sh", NULL, NULL, "SimpleVS", "vs_4_0", dwShaderFlags, NULL, NULL, &pBlob, &err, NULL );

	if (FAILED(hr))
	{
		const char* message = (const char*)err->GetBufferPointer();
		MessageBoxA(0, message, "Error happens!", MB_OK);
		return S_FALSE;
	}
	// Create the vertex shader
	hr = md3dDevice->CreateVertexShader( (DWORD*)pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &pVertexShader );

	hr = md3dDevice->CreateInputLayout( layout, 3, pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &pVertexLayout );
	//SAFE_RELEASE(pBlob);
	//SAFE_RELEASE(err);

	hr = D3DX11CompileFromFile(L"metaballs.sh", NULL, NULL, "SimplePS", "ps_4_0", dwShaderFlags, NULL, NULL, &pBlob.p, &err.p, NULL );
	if (FAILED(hr))
	{
		const char* message = (const char*)err->GetBufferPointer();
		MessageBoxA(0, message, "Error happens!", MB_OK);
		return S_FALSE;
	}	
	// Create the pixel shader
	hr = md3dDevice->CreatePixelShader( (DWORD*)pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &pPixelShader );
	//SAFE_RELEASE( pBlob );

	const char* CSTarget = (md3dDevice->GetFeatureLevel() >= D3D_FEATURE_LEVEL_11_0)? "cs_5_0" : "cs_4_0"; 
	
	V_RETURN( CompileShaderFromFile( L"metaCS.hlsl", "BuildGridCS", CSTarget, &pBlob.p ) );
    V_RETURN( md3dDevice->CreateComputeShader( pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &g_pBuildGridCS ) );
    //SAFE_RELEASE( pBlob );
    DXUT_SetDebugName( g_pBuildGridCS, "BuildGridCS" );

    // Sort Shaders
	V_RETURN( CompileShaderFromFile( L"sorter.hlsl", "BitonicSort", CSTarget, &pBlob.p ) );
    V_RETURN( md3dDevice->CreateComputeShader( pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &g_pSortBitonic ) );
    //SAFE_RELEASE( pBlob );
    DXUT_SetDebugName( g_pSortBitonic, "BitonicSort" );

	V_RETURN( CompileShaderFromFile( L"sorter.hlsl", "MatrixTranspose", CSTarget, &pBlob.p ) );
    V_RETURN( md3dDevice->CreateComputeShader( pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &g_pSortTranspose ) );
    //SAFE_RELEASE( pBlob );
    DXUT_SetDebugName( g_pSortTranspose, "MatrixTranspose" );
	
	V_RETURN( CompileShaderFromFile( L"metaCS.hlsl", "BuildGridIndicesCS", CSTarget, &pBlob.p ) );
    V_RETURN( md3dDevice->CreateComputeShader( pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &g_pBuildGridIndicesCS ) );
    //SAFE_RELEASE( pBlob );
    DXUT_SetDebugName( g_pBuildGridIndicesCS, "BuildGridIndicesCS" );

	V_RETURN( CompileShaderFromFile( L"metaCS.hlsl", "RearrangeParticlesCS", CSTarget, &pBlob.p ) );
    V_RETURN( md3dDevice->CreateComputeShader( pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &g_pRearrangeParticlesCS ) );
    //SAFE_RELEASE( pBlob );
    DXUT_SetDebugName( g_pRearrangeParticlesCS, "RearrangeParticlesCS" );

	V_RETURN( CreateConstantBuffer< SortCB >( md3dDevice, &g_pSortCB ) );
	DXUT_SetDebugName( g_pSortCB, "Sort" );

	//SAFE_RELEASE(pBlob);
	//SAFE_RELEASE(err);
	return hr;
}

void gMetaballs::drawBox()
{
	UINT strides[1] = { sizeof( Vertex ) };
	UINT offsets[1] = {0};
	md3dContext->IASetVertexBuffers( 0, 1, &mVB.p, strides, offsets );
	md3dContext->IASetInputLayout( pVertexLayout );
	md3dContext->IASetIndexBuffer( mIB, DXGI_FORMAT_R32_UINT, 0 );
	md3dContext->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	md3dContext->VSSetConstantBuffers( 0, 1, &mCB.p);
	md3dContext->OMSetBlendState(pBlendState.p, 0, 0xffffffff);

	md3dContext->OMSetDepthStencilState(pDepthStencilState.p, 0);	

	md3dContext->VSSetShader( pVertexShader.p, NULL, 0);
	md3dContext->GSSetShader( NULL, NULL, 0);
	md3dContext->PSSetShader( pPixelShader.p, NULL, 0);

	// draw to front	
	// backup backbuffer in temp
	CComPtr<ID3D11RenderTargetView> pmBackBufferView;
	CComPtr<ID3D11DepthStencilView> pmDepthStencilView;	

	md3dContext->OMGetRenderTargets(1, &pmBackBufferView.p, &pmDepthStencilView.p);
	// set pFrontS to render target		
	md3dContext->OMSetRenderTargets(1, &pFrontSView.p, pDepthStencilView);
	md3dContext->ClearDepthStencilView(pDepthStencilView, D3D10_CLEAR_DEPTH|D3D10_CLEAR_STENCIL, 1.0f, 0);
	md3dContext->ClearRenderTargetView(pFrontSView, BLACK);

	md3dContext->RSSetState(pRasterizerStateFront);

	md3dContext->DrawIndexed( mNumIndices, 0, 0 );

	// draw to back
	// set pBackS to render target
	md3dContext->OMSetRenderTargets(1, &pBackSView.p, pDepthStencilView);
	md3dContext->ClearDepthStencilView(pDepthStencilView, D3D10_CLEAR_DEPTH|D3D10_CLEAR_STENCIL, 1.0f, 0);
	md3dContext->ClearRenderTargetView(pBackSView, BLACK);

	md3dContext->RSSetState(pRasterizerStateBack);

	md3dContext->DrawIndexed( mNumIndices, 0, 0 );

	md3dContext->OMSetRenderTargets(1, &pmBackBufferView.p, pmDepthStencilView);

	//SAFE_RELEASE(pmBackBufferView);	
	//SAFE_RELEASE(pmDepthStencilView);
}

void gMetaballs::draw()
{
	drawBox();
	D3DXMATRIX ident;
	D3DXMatrixIdentity(&ident);
	quad.onFrameMove(ident, pFrontSRV, pBackSRV);
	quad.setVolume(volumeSRV);
	
	CComPtr<ID3D11RenderTargetView> pRTV = DXUTGetD3D11RenderTargetView();	
	CComPtr<ID3D11Resource> backbufferRes;
	pRTV->GetResource(&backbufferRes.p);
	D3D11_RENDER_TARGET_VIEW_DESC rtDesc;
	pRTV->GetDesc(&rtDesc);

	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.ArraySize = 1;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.Width = DXUTGetWindowWidth();
	texDesc.Height = DXUTGetWindowHeight();
	texDesc.MipLevels = 1;
	texDesc.MiscFlags = 0;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;

	CComPtr<ID3D11Texture2D> texture;
	HR( md3dDevice->CreateTexture2D(&texDesc, 0, &texture) );
	md3dContext->CopyResource(texture, backbufferRes);
	
	CComPtr<ID3D11ShaderResourceView> pBackgroundSRV;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));	
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2DArray.MostDetailedMip = 0;
	srvDesc.Texture2DArray.MipLevels = texDesc.MipLevels;
	srvDesc.Texture2DArray.FirstArraySlice = 0;
	srvDesc.Texture2DArray.ArraySize = 0;

	md3dDevice->CreateShaderResourceView(texture, &srvDesc, &pBackgroundSRV);

	quad.setBackground(pBackgroundSRV);
	quad.draw();
	
	//SAFE_RELEASE(texture);
	//SAFE_RELEASE(backbufferRes);
	//SAFE_RELEASE(pBackgroundSRV);
}

void gMetaballs::onFrameMove(D3DXMATRIX& mWorldViewProj)
{
	// Update the Constant Buffer
	VS_CONSTANT_BUFFER* pConstData;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	md3dContext->Map(mCB, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mappedResource);
	pConstData = (VS_CONSTANT_BUFFER*) mappedResource.pData;
	pConstData->mWorldViewProj = mWorldViewProj;
	md3dContext->Unmap(mCB, 0);
}

HRESULT gMetaballs::CreateVolumeBuffer()
{
	HRESULT hr = S_OK;
	//SAFE_RELEASE(p_Volume);
	//SAFE_RELEASE(p_VolumeSRV);
	//SAFE_RELEASE(p_VolumeUAV);
	int size = volumeResolution *  volumeResolution * volumeResolution;
	float* volume = new float[volumeResolution *  volumeResolution * volumeResolution];
	memset(volume, 0, sizeof(float) * size);
	V_RETURN(CreateStructuredBuffer<float>(md3dDevice, size, &p_Volume, &p_VolumeSRV, &p_VolumeUAV, volume));
	delete [] volume;
	return S_OK;	
}

HRESULT gMetaballs::CreateParticleBuffer(const Particle* p, int particleCount)
{
	HRESULT hr = S_OK;
	//SAFE_RELEASE(p_Particles);
	//SAFE_RELEASE(p_ParticlesSRV);
	//SAFE_RELEASE(p_ParticlesUAV);
	gParticle* particles = new gParticle[particleCount];
	for (int i = 0; i < particleCount; ++i)
	{
		particles[i].pos.x = p[i].position.x;
		particles[i].pos.y = p[i].position.y;
		particles[i].pos.z = p[i].position.z;
	}
	if ( particleCount == 0)
		V_RETURN(CreateStructuredBuffer<gParticle>(md3dDevice, 1, &p_Particles, &p_ParticlesSRV, &p_ParticlesUAV, NULL))
	else
		V_RETURN(CreateStructuredBuffer<gParticle>(md3dDevice, particleCount, &p_Particles, &p_ParticlesSRV, &p_ParticlesUAV, particles));
	delete [] particles;
	return S_OK;	
}

HRESULT gMetaballs::CreateGridBuffer()
{
	HRESULT hr = S_OK;
	//SAFE_RELEASE(p_Grid);
	//SAFE_RELEASE(p_GridSRV);
	//SAFE_RELEASE(p_GridUAV);
	Item* data = new Item[256 * 256 * 256];
	V_RETURN(CreateStructuredBuffer<Item>(md3dDevice, 256*256*256, &p_Grid, &p_GridSRV, &p_GridUAV, data));
	delete [] data;
	return S_OK;	
}

HRESULT gMetaballs::CreateIndexBuffer()
{
	HRESULT hr = S_OK;
	//SAFE_RELEASE(p_Index);
	//SAFE_RELEASE(p_IndexSRV);
	//SAFE_RELEASE(p_IndexUAV);
	UINT2* data = new UINT2[256 * 256 * 256];
	V_RETURN(CreateStructuredBuffer<UINT2>(md3dDevice, 256*256*256, &p_Index, &p_IndexSRV, &p_IndexUAV, data));
	delete [] data;
	return S_OK;	
}

HRESULT gMetaballs::CreateTempBuffer()
{
	HRESULT hr = S_OK;
	//SAFE_RELEASE(p_Grid);
	//SAFE_RELEASE(p_GridSRV);
	//SAFE_RELEASE(p_GridUAV);
	Item* data = new Item[256 * 256 * 256];
	V_RETURN(CreateStructuredBuffer<Item>(md3dDevice, 256*256*256, &g_pGridPingPong, &g_pGridPingPongSRV, &g_pGridPingPongUAV, data));
	delete [] data;
	return S_OK;	
}