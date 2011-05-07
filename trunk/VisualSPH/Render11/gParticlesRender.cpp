#include "DXUT.h"
#include "gParticlesRender.h"


gParticlesRender::gParticlesRender(void)
{
}


gParticlesRender::~gParticlesRender(void)
{
}

HRESULT gParticlesRender::init(CComPtr<ID3D11Device> device, CComPtr<ID3D11DeviceContext> md3dContext)
{
	HRESULT hr;
	md3dDevice = device;
	this->md3dContext = md3dContext;
	DWORD dwShaderFlags = 0;
#if defined( DEBUG ) || defined( _DEBUG )
	dwShaderFlags |= D3D11_SHADER_DEBUG_REG_INTERFACE_POINTERS;
#endif
	const D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,  D3D11_INPUT_PER_VERTEX_DATA, 0 },		
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,  D3D11_INPUT_PER_VERTEX_DATA, 0 },		
	};
	int numElements = sizeof(layout) / sizeof(layout[0]);
	mNumVertices = 0;
	mNumFaces = 0; 

	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_DEFAULT;
	vbd.ByteWidth = sizeof(VertexParticle);
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	HR(md3dDevice->CreateBuffer(&vbd, NULL, &mVB));

	// Create the index buffer
	mNumIndices = 0;	

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.ByteWidth = sizeof(DWORD);
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	HR(md3dDevice->CreateBuffer(&ibd, NULL, &mIB));

	// Create a constant buffer
	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof( CONSTANT_BUFFER );
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	HR(md3dDevice->CreateBuffer( &cbDesc, NULL, &mCB));
	cbDesc.ByteWidth = sizeof( CONSTANT_BUFFER_IMMUTE );	
	HR(md3dDevice->CreateBuffer( &cbDesc, NULL, &mCBImmute));
	
	CONSTANT_BUFFER_IMMUTE* pConstData;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	md3dContext->Map(mCBImmute, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mappedResource);
	pConstData = (CONSTANT_BUFFER_IMMUTE*) mappedResource.pData;
	
	pConstData->g_positions[0] = D3DXVECTOR4(-1,  1, 0, 0);
    pConstData->g_positions[1] = D3DXVECTOR4( 1,  1, 0, 0);
    pConstData->g_positions[3] = D3DXVECTOR4( 1, -1, 0, 0);    
    pConstData->g_positions[2] = D3DXVECTOR4(-1, -1, 0, 0);
	pConstData->g_texcoords[0] = D3DXVECTOR4( 0, 1, 0, 0); 
	pConstData->g_texcoords[1] = D3DXVECTOR4( 1, 1, 0, 0);
	pConstData->g_texcoords[2] = D3DXVECTOR4( 0, 0, 0, 0);
	pConstData->g_texcoords[3] = D3DXVECTOR4( 1, 0, 0, 0);
	
	md3dContext->Unmap(mCBImmute, 0);


	// Create a blend state
	D3D11_BLEND_DESC BlendState;
	ZeroMemory( &BlendState, sizeof( D3D11_BLEND_DESC ) );
	BlendState.AlphaToCoverageEnable = TRUE;
	BlendState.RenderTarget[0].RenderTargetWriteMask = 0x0F;
	HR(md3dDevice->CreateBlendState(&BlendState, &pBlendState));

	// Create a rasterizer state to disable culling
	D3D11_RASTERIZER_DESC RSDesc;	
	RSDesc.FillMode = D3D11_FILL_SOLID;
	RSDesc.CullMode = D3D11_CULL_NONE;
	RSDesc.FrontCounterClockwise = TRUE;
	RSDesc.DepthBias = 0;
	RSDesc.DepthBiasClamp = 0;
	RSDesc.SlopeScaledDepthBias = 0;
	RSDesc.ScissorEnable = FALSE;
	RSDesc.MultisampleEnable = TRUE;
	RSDesc.AntialiasedLineEnable = FALSE;
	HR(md3dDevice->CreateRasterizerState(&RSDesc, &pRasterizerState));

	// Create a depth stencil state to enable less-equal depth testing
	D3D11_DEPTH_STENCIL_DESC DSDesc;
	ZeroMemory( &DSDesc, sizeof( D3D11_DEPTH_STENCIL_DESC ) );
	DSDesc.DepthEnable = TRUE;
	DSDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	DSDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	HR(md3dDevice->CreateDepthStencilState( &DSDesc, &pDepthStencilState));

	// Compile the vertex shader from the file
	CComPtr<ID3DBlob> pBlob;
	CComPtr<ID3DBlob> err;
	hr = D3DX11CompileFromFile(L"particles.sh", NULL, NULL, "VS", "vs_4_0", dwShaderFlags, NULL, NULL, &pBlob, &err, NULL );

	if (FAILED(hr))
	{
		const char* message = (const char*)err->GetBufferPointer();
		MessageBoxA(0, message, "Error happens! : Vertex shader", MB_OK);
		return S_FALSE;
	}
	// Create the vertex shader
	HR(md3dDevice->CreateVertexShader( (DWORD*)pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &pVertexShader ));
	// Create input layout
	HR(md3dDevice->CreateInputLayout( layout, numElements, pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &pVertexLayout ));
	//SAFE_RELEASE(pBlob);
	//SAFE_RELEASE(err);
	
	//Compile the geometry shader from the file
	hr = D3DX11CompileFromFile(L"particles.sh", NULL, NULL, "GS", "gs_4_0", dwShaderFlags, NULL, NULL, &pBlob.p, &err.p, NULL );

	if (FAILED(hr))
	{
		const char* message = (const char*)err->GetBufferPointer();
		MessageBoxA(0, message, "Error happens! : Geometry shader", MB_OK);
		return S_FALSE;
	}
	// Create the geometry shader
	HR(md3dDevice->CreateGeometryShader( (DWORD*)pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &pGeometryShader));
	//SAFE_RELEASE(pBlob);
	//SAFE_RELEASE(err);
	
	// Compile the pixel shader from file
	HR(D3DX11CompileFromFile(L"particles.sh", NULL, NULL, "PS", "ps_4_0", dwShaderFlags, NULL, NULL, &pBlob.p, &err.p, NULL ));
	if (FAILED(hr))
	{
		const char* message = (const char*)err->GetBufferPointer();
		MessageBoxA(0, message, "Error happens! : Pixel shader", MB_OK);
		return S_FALSE;
	}	
	// Create the pixel shader
	HR(md3dDevice->CreatePixelShader( (DWORD*)pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &pPixelShader ));
	//SAFE_RELEASE(pBlob);
	//SAFE_RELEASE(err);
	// Create sampler state
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;	
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER; //D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	HR(md3dDevice->CreateSamplerState(&samplerDesc, &pSamplerState));

	// Load particle texture
	HR(D3DX11CreateShaderResourceViewFromFile(md3dDevice, L"particle.dds", NULL, NULL, &pTexParticleSRV.p, &hr));

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
	
	md3dContext->IASetVertexBuffers(0, 1, &mVB.p, strides, offsets);
	md3dContext->IASetInputLayout(pVertexLayout.p);
	md3dContext->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);
	md3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);	
	md3dContext->GSSetConstantBuffers(0, 1, &mCB.p);
	md3dContext->GSSetConstantBuffers(1, 1, &mCBImmute.p);
	//md3dDevice->PSSetConstantBuffers(0, 1, &mCB);	

	md3dContext->OMSetBlendState(pBlendState.p, 0, 0xFFFFFFFF);
	md3dContext->OMSetDepthStencilState(pDepthStencilState.p, 0);	
	md3dContext->RSSetState(pRasterizerState.p);

	md3dContext->PSSetShaderResources(0, 1, &pTexParticleSRV.p);
	md3dContext->PSSetSamplers(0, 1, &pSamplerState);

	md3dContext->VSSetShader(pVertexShader.p, NULL, 0);
	md3dContext->GSSetShader(pGeometryShader.p, NULL, 0);
	md3dContext->PSSetShader(pPixelShader.p, NULL, 0);

	md3dContext->Draw(mNumVertices, 0);
}

void gParticlesRender::updateParticles(const vector<Particle>& particles, float scale)
{
	//SAFE_RELEASE(mVB);	
	mNumVertices = particles.size();
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_DEFAULT;
	vbd.ByteWidth = sizeof(VertexParticle) * mNumVertices;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;	
	vector<VertexParticle> vertices(particles.size());
	for (size_t i = 0; i < particles.size(); ++i)
	{
		vertices[i].pos = particles[i].position * scale * 0.2;
		vertices[i].color = BLUE;
	}
	vinitData.pSysMem = &vertices[0];
	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mVB.p));	
}

void gParticlesRender::onFrameMove(D3DXMATRIX WorldViewProj, D3DXMATRIX View)
{
	D3DXMATRIX inv;
	D3DXMatrixInverse(&inv, NULL, &View);
	// Update the Constant Buffer
	CONSTANT_BUFFER* pConstData;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	md3dContext->Map(mCB, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mappedResource);
	pConstData = (CONSTANT_BUFFER*) mappedResource.pData;
	pConstData->mWorldViewProj = WorldViewProj;
	pConstData->mInverseView = inv;
	md3dContext->Unmap(mCB, 0);
}
