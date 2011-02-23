#include "DXUT.h"
#include "gParticlesRender.h"


gParticlesRender::gParticlesRender(void)
{
}


gParticlesRender::~gParticlesRender(void)
{
}

HRESULT gParticlesRender::init(ID3D10Device* device)
{
	HRESULT hr;
	md3dDevice = device;
	DWORD dwShaderFlags = 0;
#if defined( DEBUG ) || defined( _DEBUG )
	dwShaderFlags |= D3D10_SHADER_DEBUG;
#endif
	const D3D10_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT,  D3D10_INPUT_PER_VERTEX_DATA, 0 },		
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT,  D3D10_INPUT_PER_VERTEX_DATA, 0 },		
	};
	int numElements = sizeof(layout) / sizeof(layout[0]);
	mNumVertices = 0;
	mNumFaces = 0; 

	D3D10_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D10_USAGE_DEFAULT;
	vbd.ByteWidth = sizeof(VertexParticle);
	vbd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	HR(md3dDevice->CreateBuffer(&vbd, NULL, &mVB));

	// Create the index buffer
	mNumIndices = 0;	

	D3D10_BUFFER_DESC ibd;
	ibd.Usage = D3D10_USAGE_DEFAULT;
	ibd.ByteWidth = sizeof(DWORD);
	ibd.BindFlags = D3D10_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	HR(md3dDevice->CreateBuffer(&ibd, NULL, &mIB));

	// Create a constant buffer
	D3D10_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof( CONSTANT_BUFFER );
	cbDesc.Usage = D3D10_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D10_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	HR(md3dDevice->CreateBuffer( &cbDesc, NULL, &mCB));
	cbDesc.ByteWidth = sizeof( CONSTANT_BUFFER_IMMUTE );	
	HR(md3dDevice->CreateBuffer( &cbDesc, NULL, &mCBImmute));
	
	CONSTANT_BUFFER_IMMUTE* pConstData;
	mCBImmute->Map( D3D10_MAP_WRITE_DISCARD, NULL, ( void** )&pConstData );
	pConstData->g_positions[0] = D3DXVECTOR4(-1,  1, 0, 0);
    pConstData->g_positions[1] = D3DXVECTOR4( 1,  1, 0, 0);
    pConstData->g_positions[3] = D3DXVECTOR4( 1, -1, 0, 0);    
    pConstData->g_positions[2] = D3DXVECTOR4(-1, -1, 0, 0);
	pConstData->g_texcoords[0] = D3DXVECTOR4( 0, 1, 0, 0); 
	pConstData->g_texcoords[1] = D3DXVECTOR4( 1, 1, 0, 0);
	pConstData->g_texcoords[2] = D3DXVECTOR4( 0, 0, 0, 0);
	pConstData->g_texcoords[3] = D3DXVECTOR4( 1, 0, 0, 0);
	mCBImmute->Unmap();   


	// Create a blend state to disable alpha blending
	D3D10_BLEND_DESC BlendState;
	ZeroMemory( &BlendState, sizeof( D3D10_BLEND_DESC ) );
	BlendState.AlphaToCoverageEnable = TRUE;
	BlendState.BlendEnable[0] = FALSE;
	BlendState.RenderTargetWriteMask[0] = D3D10_COLOR_WRITE_ENABLE_ALL;
	HR(md3dDevice->CreateBlendState(&BlendState, &pBlendState));

	// Create a rasterizer state to disable culling
	D3D10_RASTERIZER_DESC RSDesc;
	RSDesc.FillMode = D3D10_FILL_SOLID;
	RSDesc.CullMode = D3D10_CULL_NONE;
	RSDesc.FrontCounterClockwise = TRUE;
	RSDesc.DepthBias = 0;
	RSDesc.DepthBiasClamp = 0;
	RSDesc.SlopeScaledDepthBias = 0;
	RSDesc.ScissorEnable = FALSE;
	RSDesc.MultisampleEnable = TRUE;
	RSDesc.AntialiasedLineEnable = FALSE;
	HR(md3dDevice->CreateRasterizerState(&RSDesc, &pRasterizerState));

	// Create a depth stencil state to enable less-equal depth testing
	D3D10_DEPTH_STENCIL_DESC DSDesc;
	ZeroMemory( &DSDesc, sizeof( D3D10_DEPTH_STENCIL_DESC ) );
	DSDesc.DepthEnable = TRUE;
	DSDesc.DepthFunc = D3D10_COMPARISON_LESS_EQUAL;
	DSDesc.DepthWriteMask = D3D10_DEPTH_WRITE_MASK_ALL;
	HR(md3dDevice->CreateDepthStencilState( &DSDesc, &pDepthStencilState));

	// Compile the vertex shader from the file
	ID3D10Blob* err;
	hr = D3DX10CompileFromFile(L"particles.sh", NULL, NULL, "VS", "vs_4_0", dwShaderFlags, NULL, NULL, &pBlob, &err, NULL );

	if (FAILED(hr))
	{
		const char* message = (const char*)err->GetBufferPointer();
		MessageBoxA(0, message, "Error happens! : Vertex shader", MB_OK);
		return S_FALSE;
	}
	// Create the vertex shader
	HR(md3dDevice->CreateVertexShader( (DWORD*)pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &pVertexShader ));
	// Create input layout
	HR(md3dDevice->CreateInputLayout( layout, numElements, pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &pVertexLayout ));
	SAFE_RELEASE(pBlob);
	SAFE_RELEASE(err);
	
	//Compile the geometry shader from the file
	hr = D3DX10CompileFromFile(L"particles.sh", NULL, NULL, "GS", "gs_4_0", dwShaderFlags, NULL, NULL, &pBlob, &err, NULL );

	if (FAILED(hr))
	{
		const char* message = (const char*)err->GetBufferPointer();
		MessageBoxA(0, message, "Error happens! : Geometry shader", MB_OK);
		return S_FALSE;
	}
	// Create the geometry shader
	HR(md3dDevice->CreateGeometryShader( (DWORD*)pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &pGeometryShader));
	SAFE_RELEASE(pBlob);
	SAFE_RELEASE(err);
	
	// Compile the pixel shader from file
	HR(D3DX10CompileFromFile(L"particles.sh", NULL, NULL, "PS", "ps_4_0", dwShaderFlags, NULL, NULL, &pBlob, &err, NULL ));
	if (FAILED(hr))
	{
		const char* message = (const char*)err->GetBufferPointer();
		MessageBoxA(0, message, "Error happens! : Pixel shader", MB_OK);
		return S_FALSE;
	}	
	// Create the pixel shader
	HR(md3dDevice->CreatePixelShader( (DWORD*)pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &pPixelShader ));
	SAFE_RELEASE(pBlob);
	SAFE_RELEASE(err);
	// Create sampler state
	D3D10_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D10_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D10_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressV = D3D10_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressW = D3D10_TEXTURE_ADDRESS_BORDER;	
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D10_COMPARISON_NEVER; //D3D10_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D10_FLOAT32_MAX;
	HR(md3dDevice->CreateSamplerState(&samplerDesc, &pSamplerState));

	// Load particle texture
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, L"particle.dds", NULL, NULL, &pTexParticleSRV, &hr));

	return hr;
}

void gParticlesRender::draw()
{
	if (mNumVertices == 0)
	{
		return;
	}
	UINT strides[1] = {sizeof(VertexParticle)};
	UINT offsets[1] = {0};
	md3dDevice->IASetVertexBuffers(0, 1, &mVB, strides, offsets);
	md3dDevice->IASetInputLayout(pVertexLayout);
	md3dDevice->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);
	md3dDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);	
	md3dDevice->GSSetConstantBuffers(0, 1, &mCB);
	md3dDevice->GSSetConstantBuffers(1, 1, &mCBImmute);
	//md3dDevice->PSSetConstantBuffers(0, 1, &mCB);	

	md3dDevice->OMSetBlendState(pBlendState, 0, 0xffffffff);
	md3dDevice->OMSetDepthStencilState(pDepthStencilState, 0);	
	md3dDevice->RSSetState(pRasterizerState);

	md3dDevice->PSSetShaderResources(0, 1, &pTexParticleSRV);
	md3dDevice->PSSetSamplers(0, 1, &pSamplerState);

	md3dDevice->VSSetShader(pVertexShader);
	md3dDevice->GSSetShader(pGeometryShader);
	md3dDevice->PSSetShader(pPixelShader);

	md3dDevice->Draw(mNumVertices, 0);
}

void gParticlesRender::updateParticles(const vector<Particle>& particles)
{
	SAFE_RELEASE(mVB);
	mNumVertices = particles.size();
	D3D10_BUFFER_DESC vbd;
	vbd.Usage = D3D10_USAGE_DEFAULT;
	vbd.ByteWidth = sizeof(VertexParticle) * mNumVertices;
	vbd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	D3D10_SUBRESOURCE_DATA vinitData;	
	vector<VertexParticle> vertices(particles.size());
	for (size_t i = 0; i < particles.size(); ++i)
	{
		vertices[i].pos = particles[i].position;
		// TODO : add gradient
		vertices[i].color = BLUE;
	}
	vinitData.pSysMem = &vertices[0];
	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mVB));	
}

void gParticlesRender::onFrameMove(D3DXMATRIX WorldViewProj, D3DXMATRIX View)
{
	D3DXMATRIX inv;
	float det = D3DXMatrixDeterminant(&View);
	D3DXMatrixInverse(&inv, &det, &View);
	CONSTANT_BUFFER* pConstData;
	mCB->Map( D3D10_MAP_WRITE_DISCARD, NULL, ( void** )&pConstData );
	pConstData->mWorldViewProj = WorldViewProj;
	pConstData->mInverseView = inv;
	mCB->Unmap();
}