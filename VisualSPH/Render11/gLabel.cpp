#include "DXUT.h"
#include "gLabel.h"


gLabel::gLabel(void)
{
}


gLabel::~gLabel(void)
{
	SAFE_RELEASE(mVB);
	SAFE_RELEASE(mIB);
	SAFE_RELEASE(pVertexShader);
	SAFE_RELEASE(pVertexLayout);
	SAFE_RELEASE(pPixelShader);
	SAFE_RELEASE(pDepthStencilState);
	SAFE_RELEASE(pRasterizerState);
	SAFE_RELEASE(pBlendState);
	SAFE_RELEASE(mCB);
}

void gLabel::setPosition(float x, float y, float z, float height, float width)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	md3dContext->Map(mVB, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mappedResource);
	Vertex* v = (Vertex*) mappedResource.pData;
	v->pos = D3DXVECTOR4(x, y, z, 1.0f);	
	v->height = height;
	v->width = width;
	md3dContext->Unmap(mVB, 0);
}

void gLabel::init(ID3D11Device* device, ID3D11DeviceContext* md3dContext, LPCWSTR textFileName)
{
	md3dDevice = device;
	this->md3dContext = md3dContext;
	mNumVertices = 1;
	primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;

	// Create vertex buffer
	Vertex vertices[] =
	{
		{D3DXVECTOR4(0.0f, 0.0f, 0.0f, 1.0f), 1.0f, 1.0f},
	};

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = sizeof(Vertex) * mNumVertices;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = vertices;
	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mVB));
	
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
	
	pConstData->g_texcoords[2] = D3DXVECTOR4( 0, 1, 0, 0); 
	pConstData->g_texcoords[3] = D3DXVECTOR4( 1, 1, 0, 0);
	pConstData->g_texcoords[0] = D3DXVECTOR4( 0, 0, 0, 0);
	pConstData->g_texcoords[1] = D3DXVECTOR4( 1, 0, 0, 0);
	
	md3dContext->Unmap(mCBImmute, 0);
		// Create a blend state to disable alpha blending
	D3D11_BLEND_DESC BlendState;
	ZeroMemory( &BlendState, sizeof( D3D11_BLEND_DESC ) );
	BlendState.AlphaToCoverageEnable = TRUE;
	BlendState.RenderTarget[0].RenderTargetWriteMask = 0x0F;
	//BlendState.AlphaToCoverageEnable = FALSE;
	//BlendState.RenderTarget[0].BlendEnable = TRUE;
	//BlendState.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	//BlendState.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	//BlendState.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	//BlendState.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	//BlendState.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	//BlendState.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	//BlendState.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;	
	md3dDevice->CreateBlendState( &BlendState, &pBlendState ) ;

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
	RSDesc.AntialiasedLineEnable = FALSE;
	md3dDevice->CreateRasterizerState( &RSDesc, &pRasterizerState );

	// Create a depth stencil state to enable less-equal depth testing
	D3D11_DEPTH_STENCIL_DESC DSDesc;
	ZeroMemory( &DSDesc, sizeof( D3D11_DEPTH_STENCIL_DESC ) );
	DSDesc.DepthEnable = TRUE;
	DSDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	DSDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	md3dDevice->CreateDepthStencilState( &DSDesc, &pDepthStencilState );


	// Compile the vertex shader from the file
	ID3DBlob*	pBlob;
	ID3DBlob*	err;
	HRESULT hr;
	md3dDevice = device;
	DWORD dwShaderFlags = 0;
#if defined( DEBUG ) || defined( _DEBUG )
	dwShaderFlags |= D3D11_SHADER_DEBUG_REG_INTERFACE_POINTERS;
#endif
	const D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,  D3D11_INPUT_PER_VERTEX_DATA, 0 },		
		{ "WIDTH", 0, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,  D3D11_INPUT_PER_VERTEX_DATA, 0 },		
		{ "HEIGHT", 0, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,  D3D11_INPUT_PER_VERTEX_DATA, 0 },		
	};
	int numElements = sizeof(layout) / sizeof(layout[0]);
	hr = D3DX11CompileFromFile(L"label.hlsl", NULL, NULL, "VS", "vs_4_0", dwShaderFlags, NULL, NULL, &pBlob, &err, NULL );
	if (FAILED(hr))
	{
		const char* message = (const char*)err->GetBufferPointer();
		MessageBoxA(0, message, "Error happens!", MB_OK);		
	}
	// Create the vertex shader
	hr = md3dDevice->CreateVertexShader( (DWORD*)pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &pVertexShader );
	hr = md3dDevice->CreateInputLayout( layout, numElements, pBlob->GetBufferPointer(),	pBlob->GetBufferSize(), &pVertexLayout );
	SAFE_RELEASE(pBlob);
	SAFE_RELEASE(err);

	//Compile the geometry shader from the file
	hr = D3DX11CompileFromFile(L"label.hlsl", NULL, NULL, "GS", "gs_4_0", dwShaderFlags, NULL, NULL, &pBlob, &err, NULL );
	if (FAILED(hr))
	{
		const char* message = (const char*)err->GetBufferPointer();
		MessageBoxA(0, message, "Error happens! : Geometry shader", MB_OK);
	}
	// Create the geometry shader
	HR(md3dDevice->CreateGeometryShader( (DWORD*)pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &pGeometryShader));
	SAFE_RELEASE(pBlob);
	SAFE_RELEASE(err);

	hr = D3DX11CompileFromFile(L"label.hlsl", NULL, NULL, "PS", "ps_4_0", dwShaderFlags, NULL, NULL, &pBlob, &err, NULL );
	if (FAILED(hr))
	{
		const char* message = (const char*)err->GetBufferPointer();
		MessageBoxA(0, message, "Error happens!", MB_OK);
	}	
	// Create the pixel shader
	hr = md3dDevice->CreatePixelShader( (DWORD*)pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &pPixelShader );
	SAFE_RELEASE(pBlob);
	SAFE_RELEASE(err);
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
	HR(D3DX11CreateShaderResourceViewFromFile(md3dDevice, textFileName, NULL, NULL, &pTextSRV, &hr));
}

void gLabel::draw()
{
	md3dContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	md3dContext->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
	md3dContext->IASetInputLayout(pVertexLayout);
	md3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);	
	md3dContext->GSSetConstantBuffers(0, 1, &mCB);
	md3dContext->GSSetConstantBuffers(1, 1, &mCBImmute);

	md3dContext->OMSetBlendState(pBlendState, 0, 0xFFFFFFFF);
	md3dContext->OMSetDepthStencilState(pDepthStencilState, 0);	
	md3dContext->RSSetState(pRasterizerState);

	md3dContext->PSSetShaderResources(0, 1, &pTextSRV);
	md3dContext->PSSetSamplers(0, 1, &pSamplerState);
	md3dContext->VSSetShader(pVertexShader, NULL, 0);
	md3dContext->GSSetShader(pGeometryShader, NULL, 0);
	md3dContext->PSSetShader(pPixelShader, NULL, 0);

	md3dContext->Draw(mNumVertices, 0);
}

void gLabel::onFrameMove(D3DXMATRIX WorldViewProj, D3DXMATRIX View)
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