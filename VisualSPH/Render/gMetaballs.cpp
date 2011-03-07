#include "DXUT.h"
#include "gMetaballs.h"


gMetaballs::gMetaballs(void)
{
}

gMetaballs::~gMetaballs(void)
{
	SAFE_RELEASE(pVertexShader);
	SAFE_RELEASE(pVertexLayout);
	SAFE_RELEASE(pPixelShader);

	SAFE_RELEASE(mCB);
	SAFE_RELEASE(pBlendState);
	SAFE_RELEASE(pRasterizerStateFront);
	SAFE_RELEASE(pRasterizerStateBack);
	SAFE_RELEASE(pDepthStencilState);	
	SAFE_RELEASE(pFrontS);
	SAFE_RELEASE(pFrontSView);
	SAFE_RELEASE(pBackS);
	SAFE_RELEASE(pBackSView);
	SAFE_RELEASE(pFrontSRV);
	SAFE_RELEASE(pBackSRV);
	SAFE_RELEASE(pVolume);
	SAFE_RELEASE(volumeSRV);
	SAFE_RELEASE(pNoise);
	SAFE_RELEASE(pNoiseSRV);
	SAFE_RELEASE(pRenderTargetView);
	SAFE_RELEASE(pDepthStencilBuffer);
	SAFE_RELEASE(pDepthStencilView);
}

float gMetaballs::calcMetaball(D3DXVECTOR3 centerBall, D3DXVECTOR3 cell)
{	
	D3DXVECTOR3 tmp = centerBall - cell;	
	float len = D3DXVec3Dot(&tmp, &tmp);
	if (len > metaballsSize * metaballsSize) {
		return 0.0f;
	}
	return 1.0f / (len + 1e-5f);
}

void gMetaballs::updateVolume(const vector<Particle>& particles, int numParticles, float scale, float metaballsSize)
{
	this->scale = scale;
	this->metaballsSize = metaballsSize;
	field.clear();
	//for (int i = 0; i < numParticles; ++i)
	//{
	//	int x = (int) (particles[i].position.x * scale);
	//	int y = (int) (particles[i].position.y * scale);
	//	int z = (int) (particles[i].position.z * scale);
	//	for (int dx = (int) -metaballsSize; dx <= (int) metaballsSize; ++dx)
	//	{
	//		for (int dy = (int) -metaballsSize; dy <= (int) metaballsSize; ++dy)
	//		{
	//			for (int dz = (int) -metaballsSize; dz <= (int) metaballsSize; ++dz)
	//			{
	//				D3DXVECTOR3 cell(x + dx, y + dy, z + dz);
	//				if(field.isInside(x + dx, y + dy, z + dz))
	//				{
	//					field.lvalue(x + dx, y + dy, z + dz) += calcMetaball(particles[i].position * scale, cell);
	//				}
	//			}
	//		}
	//	}
	//}
	//---------------------Changed here----------------------//
	parallel_for(blocked_range<size_t>(0, numParticles), CalcField(&field, &particles[0], metaballsSize, scale), auto_partitioner());

	D3D10_MAPPED_TEXTURE3D pMT;
	HR(pVolume->Map(D3D10CalcSubresource(0, 0, 1), D3D10_MAP_WRITE_DISCARD, 0, &pMT));	
	int strideI = pMT.DepthPitch / sizeof(float);
	int strideJ = pMT.RowPitch / sizeof(float);
	float* textureData = (float*) pMT.pData;	
	for (int i = 0; i < field.xSize; ++i)
	{
		for (int j = 0; j < field.ySize; ++j)
		{
			for (int k = 0; k < field.zSize; ++k)
			{
				textureData[i * strideI + j * strideJ + k] = field.value(i, j, k);
			}
		}		
	}
	pVolume->Unmap(D3D10CalcSubresource(0, 0, 1));
}

void gMetaballs::init(ID3D10Device* device, int _screenWidth, int _screenHeight, int _volumeResolution)
{
	md3dDevice = device;
	screenHeight = _screenHeight;
	screenWidth = _screenWidth;
	volumeResolution = _volumeResolution;	
	field = DenseField(volumeResolution, volumeResolution, volumeResolution);
	onCreate();
	createTexture2D();
	createTexture3D();
	quad.init(md3dDevice);
	quad.setNoise(pNoiseSRV);
}

void gMetaballs::onFrameResize(int width, int height)
{
	screenWidth = width;
	screenHeight = height;
	SAFE_RELEASE(pFrontS);
	SAFE_RELEASE(pFrontSRV);
	SAFE_RELEASE(pFrontSView);
	SAFE_RELEASE(pBackS);
	SAFE_RELEASE(pBackSRV);
	SAFE_RELEASE(pBackSView);
	SAFE_RELEASE(pDepthStencilBuffer);
	SAFE_RELEASE(pDepthStencilView);	
	createTexture2D();
}

HRESULT gMetaballs::createTexture2D()
{
	HRESULT hr;
	// Create the render target texture
	D3D10_TEXTURE2D_DESC desc;
	ZeroMemory( &desc, sizeof(desc) );
	// TODO: set screen resolution
	desc.Width = screenWidth;
	desc.Height = screenHeight;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D10_USAGE_DEFAULT;
	desc.BindFlags = D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE;

	hr = md3dDevice->CreateTexture2D(&desc, NULL, &pFrontS );
	hr = md3dDevice->CreateRenderTargetView(pFrontS, 0, &pFrontSView);
	hr = md3dDevice->CreateTexture2D(&desc, NULL, &pBackS );
	hr = md3dDevice->CreateRenderTargetView(pBackS, 0, &pBackSView);

	D3D10_SHADER_RESOURCE_VIEW_DESC viewDesc;
	viewDesc.Format = desc.Format;
	viewDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
	viewDesc.Texture2DArray.MostDetailedMip = 0;
	viewDesc.Texture2DArray.MipLevels = desc.MipLevels;
	viewDesc.Texture2DArray.FirstArraySlice = 0;
	viewDesc.Texture2DArray.ArraySize = 0;

	md3dDevice->CreateShaderResourceView(pFrontS, 0, &pFrontSRV);
	md3dDevice->CreateShaderResourceView(pBackS, 0, &pBackSRV);

	D3D10_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width     = screenWidth;
	depthStencilDesc.Height    = screenHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format    = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count   = 1; 
	depthStencilDesc.SampleDesc.Quality = 0; 
	depthStencilDesc.Usage          = D3D10_USAGE_DEFAULT;
	depthStencilDesc.BindFlags      = D3D10_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0; 
	depthStencilDesc.MiscFlags      = 0;

	hr = md3dDevice->CreateTexture2D(&depthStencilDesc, 0, &pDepthStencilBuffer);
	hr = md3dDevice->CreateDepthStencilView(pDepthStencilBuffer, 0, &pDepthStencilView);

	ZeroMemory( &desc, sizeof(desc) );
	desc.Width = 32;
	desc.Height = 32;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R32_FLOAT;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D10_USAGE_DEFAULT;
	desc.BindFlags = D3D10_BIND_SHADER_RESOURCE;
	D3D10_SUBRESOURCE_DATA noiseInit;
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
	HR(md3dDevice->CreateTexture2D(&desc, &noiseInit, &pNoise ));

	viewDesc.Format = desc.Format;
	viewDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
	viewDesc.Texture2DArray.MostDetailedMip = 0;
	viewDesc.Texture2DArray.MipLevels = desc.MipLevels;
	viewDesc.Texture2DArray.FirstArraySlice = 0;
	viewDesc.Texture2DArray.ArraySize = 0;

	md3dDevice->CreateShaderResourceView(pNoise, 0, &pNoiseSRV);
	SAFE_DELETE_ARRAY(pNoiseData);
	return hr;
}

HRESULT gMetaballs::createTexture3D()
{
	HRESULT hr = S_OK;
	D3D10_TEXTURE3D_DESC volume_desc;
	ZeroMemory(&volume_desc, sizeof(volume_desc));
	volume_desc.Width = volumeResolution;
	volume_desc.Height = volumeResolution;
	volume_desc.Depth = volumeResolution;	
	volume_desc.MipLevels = 1;
	volume_desc.Format = DXGI_FORMAT_R32_FLOAT;
	volume_desc.Usage = D3D10_USAGE_DYNAMIC; 
	volume_desc.BindFlags = D3D10_BIND_SHADER_RESOURCE;
	volume_desc.MiscFlags = 0;
	volume_desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;

	D3D10_SUBRESOURCE_DATA densityField;
	float* fieldData = new float[volumeResolution * volumeResolution * volumeResolution];
	
	densityField.pSysMem = fieldData;
	densityField.SysMemPitch = volumeResolution * sizeof(float);
	densityField.SysMemSlicePitch = volumeResolution * volumeResolution * sizeof(float);

	D3D10_SHADER_RESOURCE_VIEW_DESC densityFieldSRVdesc;
	ZeroMemory(&densityFieldSRVdesc, sizeof(densityFieldSRVdesc));
	densityFieldSRVdesc.Format = volume_desc.Format;
	densityFieldSRVdesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE3D;
	densityFieldSRVdesc.Texture3D.MipLevels = 1;
	densityFieldSRVdesc.Texture3D.MostDetailedMip = 0;

	HR(md3dDevice->CreateTexture3D(&volume_desc, &densityField, &pVolume));
	HR(md3dDevice->CreateShaderResourceView(pVolume, &densityFieldSRVdesc, &volumeSRV));

	SAFE_DELETE_ARRAY(fieldData);
	return hr;
}

HRESULT gMetaballs::onCreate()
{
	HRESULT hr;
	DWORD dwShaderFlags = 0;
#if defined( DEBUG ) || defined( _DEBUG )
	dwShaderFlags |= D3D10_SHADER_DEBUG;
#endif
	const D3D10_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
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

	D3D10_BUFFER_DESC vbd;
	vbd.Usage = D3D10_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex) * mNumVertices;
	vbd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	D3D10_SUBRESOURCE_DATA vinitData;
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

	D3D10_BUFFER_DESC ibd;
	ibd.Usage = D3D10_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(DWORD) * mNumIndices;
	ibd.BindFlags = D3D10_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	D3D10_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = indices;
	hr = md3dDevice->CreateBuffer(&ibd, &iinitData, &mIB);

	// Create a constant buffer
	D3D10_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof( VS_CONSTANT_BUFFER );
	cbDesc.Usage = D3D10_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D10_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	hr = md3dDevice->CreateBuffer( &cbDesc, NULL, &mCB);


	// Create a blend state to disable alpha blending
	D3D10_BLEND_DESC BlendState;
	ZeroMemory( &BlendState, sizeof( D3D10_BLEND_DESC ) );
	BlendState.BlendEnable[0] = FALSE;
	BlendState.RenderTargetWriteMask[0] = D3D10_COLOR_WRITE_ENABLE_ALL;
	hr = md3dDevice->CreateBlendState( &BlendState, &pBlendState ) ;

	// Create a rasterizer state to disable culling
	D3D10_RASTERIZER_DESC RSDesc;
	RSDesc.FillMode = D3D10_FILL_SOLID;
	RSDesc.CullMode = D3D10_CULL_FRONT;
	RSDesc.FrontCounterClockwise = TRUE;
	RSDesc.DepthBias = 0;
	RSDesc.DepthBiasClamp = 0;
	RSDesc.SlopeScaledDepthBias = 0;
	RSDesc.ScissorEnable = FALSE;
	RSDesc.MultisampleEnable = TRUE;
	RSDesc.AntialiasedLineEnable = FALSE;
	hr = md3dDevice->CreateRasterizerState( &RSDesc, &pRasterizerStateFront );
	RSDesc.CullMode = D3D10_CULL_BACK;
	hr = md3dDevice->CreateRasterizerState( &RSDesc, &pRasterizerStateBack );

	// Create a depth stencil state to enable less-equal depth testing
	D3D10_DEPTH_STENCIL_DESC DSDesc;
	ZeroMemory( &DSDesc, sizeof( D3D10_DEPTH_STENCIL_DESC ) );
	DSDesc.DepthEnable = TRUE;
	DSDesc.DepthFunc = D3D10_COMPARISON_LESS_EQUAL;
	DSDesc.DepthWriteMask = D3D10_DEPTH_WRITE_MASK_ALL;
	hr = md3dDevice->CreateDepthStencilState( &DSDesc, &pDepthStencilState );

	ID3D10Blob*	pBlob;	
	// Compile the vertex shader from the file
	ID3D10Blob* err;
	
	hr = D3DX10CompileFromFile(L"metaballs.sh", NULL, NULL, "SimpleVS", "vs_4_0", dwShaderFlags, NULL, NULL, &pBlob, &err, NULL );

	if (FAILED(hr))
	{
		const char* message = (const char*)err->GetBufferPointer();
		MessageBoxA(0, message, "Error happens!", MB_OK);
		return S_FALSE;
	}
	// Create the vertex shader
	hr = md3dDevice->CreateVertexShader( (DWORD*)pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &pVertexShader );

	hr = md3dDevice->CreateInputLayout( layout, 3, pBlob->GetBufferPointer(),
		pBlob->GetBufferSize(), &pVertexLayout );
	SAFE_RELEASE(pBlob);
	SAFE_RELEASE(err);

	hr = D3DX10CompileFromFile(L"metaballs.sh", NULL, NULL, "SimplePS", "ps_4_0", dwShaderFlags, NULL, NULL, &pBlob, &err, NULL );
	if (FAILED(hr))
	{
		const char* message = (const char*)err->GetBufferPointer();
		MessageBoxA(0, message, "Error happens!", MB_OK);
		return S_FALSE;
	}	
	// Create the pixel shader
	hr = md3dDevice->CreatePixelShader( (DWORD*)pBlob->GetBufferPointer(),
		pBlob->GetBufferSize(), &pPixelShader );
	SAFE_RELEASE(pBlob);
	SAFE_RELEASE(err);
	return hr;
}

void gMetaballs::drawBox()
{
	UINT strides[1] = { sizeof( Vertex ) };
	UINT offsets[1] = {0};
	md3dDevice->IASetVertexBuffers( 0, 1, &mVB, strides, offsets );
	md3dDevice->IASetInputLayout( pVertexLayout );
	md3dDevice->IASetIndexBuffer( mIB, DXGI_FORMAT_R32_UINT, 0 );
	md3dDevice->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	md3dDevice->VSSetConstantBuffers( 0, 1, &mCB);
	md3dDevice->OMSetBlendState(pBlendState, 0, 0xffffffff);

	md3dDevice->OMSetDepthStencilState(pDepthStencilState, 0);	

	md3dDevice->VSSetShader( pVertexShader );
	md3dDevice->GSSetShader( NULL );
	md3dDevice->PSSetShader( pPixelShader );

	// draw to front	
	// backup backbuffer in temp
	ID3D10RenderTargetView* pmBackBufferView;
	ID3D10DepthStencilView* pmDepthStencilView;	

	md3dDevice->OMGetRenderTargets(1, &pmBackBufferView, &pmDepthStencilView);
	// set pFrontS to render target		
	md3dDevice->OMSetRenderTargets(1, &pFrontSView, pDepthStencilView);
	md3dDevice->ClearDepthStencilView(pDepthStencilView, D3D10_CLEAR_DEPTH|D3D10_CLEAR_STENCIL, 1.0f, 0);
	md3dDevice->ClearRenderTargetView(pFrontSView, BLACK);

	md3dDevice->RSSetState(pRasterizerStateFront);

	md3dDevice->DrawIndexed( mNumIndices, 0, 0 );

	// draw to back
	// set pBackS to render target
	md3dDevice->OMSetRenderTargets(1, &pBackSView, pDepthStencilView);
	md3dDevice->ClearDepthStencilView(pDepthStencilView, D3D10_CLEAR_DEPTH|D3D10_CLEAR_STENCIL, 1.0f, 0);
	md3dDevice->ClearRenderTargetView(pBackSView, BLACK);

	md3dDevice->RSSetState(pRasterizerStateBack);

	md3dDevice->DrawIndexed( mNumIndices, 0, 0 );

	md3dDevice->OMSetRenderTargets(1, &pmBackBufferView, pmDepthStencilView);

	SAFE_RELEASE(pmBackBufferView);	
	SAFE_RELEASE(pmDepthStencilView);
}

void gMetaballs::draw()
{
	drawBox();
	D3DXMATRIX ident;
	D3DXMatrixIdentity(&ident);
	quad.onFrameMove(ident, pFrontSRV, pBackSRV);
	quad.setVolume(volumeSRV);
	quad.draw();
}

void gMetaballs::onFrameMove(D3DXMATRIX& mWorldViewProj)
{
	// Update the Constant Buffer
	VS_CONSTANT_BUFFER* pConstData;
	mCB->Map( D3D10_MAP_WRITE_DISCARD, NULL, ( void** )&pConstData );
	pConstData->mWorldViewProj = mWorldViewProj;
	mCB->Unmap();
}
