#include "DXUT.h"
#include "gQuad.h"
#include "ShaderUtils.h"
gQuad::gQuad(void)
{
}


gQuad::~gQuad(void)
{
	SAFE_RELEASE(pVertexShader);
	SAFE_RELEASE(pVertexLayout);
	SAFE_RELEASE(pPixelShader);
	SAFE_RELEASE(pSamplerState);
	SAFE_RELEASE(pBlendState);
	SAFE_RELEASE(pRasterizerState);
	SAFE_RELEASE(pDepthStencilState);
	SAFE_RELEASE(mCB);
	SAFE_RELEASE(mCB_Immute);
}

HRESULT gQuad::init(ID3D11Device* device, ID3D11DeviceContext* md3dContext)
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
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,  D3D11_INPUT_PER_VERTEX_DATA, 0 },		
	};
	int numElements = sizeof(layout) / sizeof(layout[0]);
	mNumVertices = 4;
	mNumFaces = 2; 
	// Create vertex buffer
	Vertex2D vertices[] =
	{
		{D3DXVECTOR3(-1, -1, 0),	D3DXVECTOR2(0.0f, 1.0f), D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f)},
		{D3DXVECTOR3(-1, 1, 0),		D3DXVECTOR2(0.0f, 0.0f), D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f)},
		{D3DXVECTOR3(1, 1, 0),		D3DXVECTOR2(1.0f, 0.0f), D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f)},
		{D3DXVECTOR3(1, -1, 0),		D3DXVECTOR2(1.0f, 1.0f), D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f)},
	};

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex2D) * mNumVertices;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = vertices;
	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mVB));

	// Create the index buffer
	mNumIndices = 6;
	DWORD indices[] = {
		0, 2, 1,
		0, 3, 2,
	};

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(DWORD) * mNumIndices;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = indices;
	HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mIB));

	// Create a constant buffer
	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof( CONSTANT_BUFFER );
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	HR(md3dDevice->CreateBuffer( &cbDesc, NULL, &mCB));
	cbDesc.ByteWidth = sizeof( CBUFFER_IMMUTE);
	HR(md3dDevice->CreateBuffer( &cbDesc, NULL, &mCB_Immute));

	CBUFFER_IMMUTE* pCBData;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	md3dContext->Map(mCB_Immute, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mappedResource);	
	pCBData = (CBUFFER_IMMUTE*) mappedResource.pData;
	pCBData->ambientLight = D3DXCOLOR(1.0f, 1.0f , 1.0f, 1.0f);

	pCBData->l1.pos = D3DXVECTOR4(-1, -1, -1, 0);
	pCBData->l1.color = D3DXCOLOR(1.0f, 1.0f , 1.0f, 1.0f);

	pCBData->l2.pos = D3DXVECTOR4(1, 1, 1, 0);
	pCBData->l2.color = D3DXCOLOR(255. / 255., 255. / 255., 255. / 255., 1.0f);

	pCBData->material.Ka = 0.1f;
	pCBData->material.Kd = 0.5f;
	pCBData->material.Ks = 0.5f;
	pCBData->material.A = 30.f;

	md3dContext->Unmap(mCB_Immute, 0);


	// Create a blend state to disable alpha blending
	D3D11_BLEND_DESC BlendState;
	ZeroMemory( &BlendState, sizeof( D3D11_BLEND_DESC ) );
	BlendState.RenderTarget[0].BlendEnable = FALSE;
	BlendState.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
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
	ID3DBlob*	pBlob;
	ID3DBlob* err;
	
	hr = D3DX11CompileFromFile(L"metaballs.sh", NULL, NULL, "QuadVS", "vs_4_0", dwShaderFlags, NULL, NULL, &pBlob, &err, NULL );

	if (FAILED(hr))
	{
		const char* message = (const char*)err->GetBufferPointer();
		MessageBoxA(0, message, "Error happens!", MB_OK);
		return S_FALSE;
	}
	// Create the vertex shader	
	HR(md3dDevice->CreateVertexShader( (DWORD*)pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &pVertexShader ));
	// Create input layout
	HR(md3dDevice->CreateInputLayout( layout, numElements, pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &pVertexLayout ));
	//HR(CreateVertexShaderAndInputLayoutFromAssembly(md3dDevice, "metaVS.vs", &pVertexShader, (D3D10_INPUT_ELEMENT_DESC*) &layout, numElements, &pVertexLayout));
	SAFE_RELEASE(pBlob);
	SAFE_RELEASE(err);
	// Compile the pixel shader from file

	hr = D3DX11CompileFromFile(L"metaballs.sh", NULL, NULL, "RayCastPS", "ps_4_0", dwShaderFlags, NULL, NULL, &pBlob, &err, NULL );
		
	if (FAILED(hr))
	{
		const char* message = (const char*)err->GetBufferPointer();
		MessageBoxA(0, message, "Error happens!", MB_OK);
		return S_FALSE;
	}	
	// Create the pixel shader
	//HR(CreatePixelShaderFromAssembly(md3dDevice, "metaPS.ps", &pPixelShader));
	HR(md3dDevice->CreatePixelShader( (DWORD*)pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &pPixelShader ));
	//HR(md3dDevice->CreatePixelShader( shaderCode, size, &pPixelShader ));
	SAFE_RELEASE(pBlob);
	SAFE_RELEASE(err);
	// Create sampler state
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;	
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 8;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER; //D3D10_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	HR(md3dDevice->CreateSamplerState(&samplerDesc, &pSamplerState));
	return hr;
}

void gQuad::draw()
{
	UINT strides[1] = { sizeof( Vertex2D ) };
	UINT offsets[1] = {0};
	md3dContext->IASetVertexBuffers(0, 1, &mVB, strides, offsets);
	md3dContext->IASetInputLayout(pVertexLayout);
	md3dContext->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);
	md3dContext->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	md3dContext->VSSetConstantBuffers( 0, 1, &mCB);
	//md3dDevice->PSSetConstantBuffers( 0, 1, &mCB);
	md3dContext->PSSetConstantBuffers( 0, 1, &mCB_Immute);
	md3dContext->OMSetBlendState(pBlendState, 0, 0xffffffff);
	md3dContext->OMSetDepthStencilState(pDepthStencilState, 0);	
	md3dContext->RSSetState(pRasterizerState);

	md3dContext->PSSetSamplers(0, 1, &pSamplerState);

	md3dContext->VSSetShader(pVertexShader, NULL, 0);
	md3dContext->GSSetShader(NULL, NULL, 0);
	md3dContext->PSSetShader(pPixelShader, NULL, 0);

	md3dContext->PSSetShaderResources(0, 1, &frontSRV);
	md3dContext->PSSetShaderResources(1, 1, &backSRV);
	md3dContext->PSSetShaderResources(2, 1, &volume);
	md3dContext->PSSetShaderResources(3, 1, &pNoiseSRV);

	md3dContext->DrawIndexed( mNumIndices, 0, 0 );
}

void gQuad::onFrameMove(D3DXMATRIX& mWorldViewProj, ID3D11ShaderResourceView* frontSRV, ID3D11ShaderResourceView* backSRV)
{
	// Update the Constant Buffer
	CONSTANT_BUFFER* pConstData;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	md3dContext->Map(mCB, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mappedResource);
	pConstData = (CONSTANT_BUFFER*) mappedResource.pData;
	pConstData->mWorldViewProj = mWorldViewProj;
	md3dContext->Unmap(mCB, 0);

	this->frontSRV = frontSRV;
	this->backSRV = backSRV;
}

void gQuad::setVolume(ID3D11ShaderResourceView* _vol)
{
	volume = _vol;
}

void gQuad::setNoise(ID3D11ShaderResourceView* _pNoiseSRV)
{
	pNoiseSRV = _pNoiseSRV;
}
