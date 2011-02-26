#include "DXUT.h"
#include "gQuad.h"

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
}

HRESULT gQuad::init(ID3D10Device* device)
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
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT,  D3D10_INPUT_PER_VERTEX_DATA, 0 },		
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

	D3D10_BUFFER_DESC vbd;
	vbd.Usage = D3D10_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex2D) * mNumVertices;
	vbd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	D3D10_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = vertices;
	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mVB));

	// Create the index buffer
	mNumIndices = 6;
	DWORD indices[] = {
		0, 2, 1,
		0, 3, 2,
	};

	D3D10_BUFFER_DESC ibd;
	ibd.Usage = D3D10_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(DWORD) * mNumIndices;
	ibd.BindFlags = D3D10_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	D3D10_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = indices;
	HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mIB));

	// Create a constant buffer
	D3D10_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof( CONSTANT_BUFFER );
	cbDesc.Usage = D3D10_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D10_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	HR(md3dDevice->CreateBuffer( &cbDesc, NULL, &mCB));

	// Create a blend state to disable alpha blending
	D3D10_BLEND_DESC BlendState;
	ZeroMemory( &BlendState, sizeof( D3D10_BLEND_DESC ) );
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
	hr = D3DX10CompileFromFile(L"metaballs.sh", NULL, NULL, "QuadVS", "vs_4_0", dwShaderFlags, NULL, NULL, &pBlob, &err, NULL );

	if (FAILED(hr))
	{
		const char* message = (const char*)err->GetBufferPointer();
		MessageBoxA(0, message, "Error happens!", MB_OK);
		return S_FALSE;
	}
	// Create the vertex shader
	HR(md3dDevice->CreateVertexShader( (DWORD*)pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &pVertexShader ));
	// Create input layout
	HR(md3dDevice->CreateInputLayout( layout, numElements, pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &pVertexLayout ));
	SAFE_RELEASE(pBlob);
	SAFE_RELEASE(err);
	// Compile the pixel shader from file
	hr = D3DX10CompileFromFile(L"metaballs.sh", NULL, NULL, "RayCastPS", "ps_4_0", dwShaderFlags, NULL, NULL, &pBlob, &err, NULL );
	if (FAILED(hr))
	{
		const char* message = (const char*)err->GetBufferPointer();
		MessageBoxA(0, message, "Error happens!", MB_OK);
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
	return hr;
}

void gQuad::draw()
{
	UINT strides[1] = { sizeof( Vertex2D ) };
	UINT offsets[1] = {0};
	md3dDevice->IASetVertexBuffers(0, 1, &mVB, strides, offsets);
	md3dDevice->IASetInputLayout(pVertexLayout);
	md3dDevice->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);
	md3dDevice->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	md3dDevice->VSSetConstantBuffers( 0, 1, &mCB);
	md3dDevice->PSSetConstantBuffers( 0, 1, &mCB);
	md3dDevice->OMSetBlendState(pBlendState, 0, 0xffffffff);
	md3dDevice->OMSetDepthStencilState(pDepthStencilState, 0);	
	md3dDevice->RSSetState(pRasterizerState);

	md3dDevice->PSSetSamplers(0, 1, &pSamplerState);

	md3dDevice->VSSetShader(pVertexShader);
	md3dDevice->GSSetShader(NULL);
	md3dDevice->PSSetShader(pPixelShader);

	md3dDevice->PSSetShaderResources(0, 1, &frontSRV);
	md3dDevice->PSSetShaderResources(1, 1, &backSRV);
	md3dDevice->PSSetShaderResources(2, 1, &volume);
	md3dDevice->PSSetShaderResources(3, 1, &pNoiseSRV);

	md3dDevice->DrawIndexed( mNumIndices, 0, 0 );
}

void gQuad::onFrameMove(D3DXMATRIX& mWorldViewProj, ID3D10ShaderResourceView* frontSRV, ID3D10ShaderResourceView* backSRV)
{
	// Update the Constant Buffer
	CONSTANT_BUFFER* pConstData;
	mCB->Map( D3D10_MAP_WRITE_DISCARD, NULL, ( void** )&pConstData );
	pConstData->mWorldViewProj = mWorldViewProj;
	pConstData->vLightPos1 = D3DXVECTOR4(-1, -1, -1, 0);
	pConstData->vLightPos2 = D3DXVECTOR4(1, 1, 1, 0);
	pConstData->vMaterial = D3DXVECTOR4(112. / 255., 147. / 255., 219. / 255., 0.8);
	mCB->Unmap();
	this->frontSRV = frontSRV;
	this->backSRV = backSRV;
}

void gQuad::setVolume(ID3D10ShaderResourceView* _vol)
{
	volume = _vol;
}

void gQuad::setNoise(ID3D10ShaderResourceView* _pNoiseSRV)
{
	pNoiseSRV = _pNoiseSRV;
}