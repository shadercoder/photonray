#include "DXUT.h"
#include "Axis.h"


Axis::Axis(void)
{
}


Axis::~Axis(void)
{
	ReleaseCOM(mVB);
	ReleaseCOM(mIB);
}

void Axis::init(ID3D10Device* device)
{
	md3dDevice = device;
	mNumVertices = 6;
	primitiveTopology = D3D10_PRIMITIVE_TOPOLOGY_LINELIST;

	// Create vertex buffer
	Vertex vertices[] =
	{
		{D3DXVECTOR4(0.0f, 0.0f, 0.0f, 1.0f) * 10.f, RED},
		{D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f) * 10.f, RED},
		{D3DXVECTOR4(0.0f, 0.0f, 0.0f, 1.0f) * 10.f, GREEN},
		{D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f) * 10.f, GREEN},
		{D3DXVECTOR4(0.0f, 0.0f, 0.0f, 1.0f) * 10.f, BLUE},
		{D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f) * 10.f, BLUE},
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
	md3dDevice->CreateBlendState( &BlendState, &pBlendState ) ;

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
	md3dDevice->CreateRasterizerState( &RSDesc, &pRasterizerState );

	// Create a depth stencil state to enable less-equal depth testing
	D3D10_DEPTH_STENCIL_DESC DSDesc;
	ZeroMemory( &DSDesc, sizeof( D3D10_DEPTH_STENCIL_DESC ) );
	DSDesc.DepthEnable = TRUE;
	DSDesc.DepthFunc = D3D10_COMPARISON_LESS_EQUAL;
	DSDesc.DepthWriteMask = D3D10_DEPTH_WRITE_MASK_ALL;
	md3dDevice->CreateDepthStencilState( &DSDesc, &pDepthStencilState );


	// Compile the vertex shader from the file
	ID3D10Blob*	pBlob;
	ID3D10Blob* err;
	HRESULT hr;
	md3dDevice = device;
	DWORD dwShaderFlags = 0;
#if defined( DEBUG ) || defined( _DEBUG )
	dwShaderFlags |= D3D10_SHADER_DEBUG;
#endif
	const D3D10_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT,  D3D10_INPUT_PER_VERTEX_DATA, 0 },		
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT,  D3D10_INPUT_PER_VERTEX_DATA, 0 },		
	};
	int numElements = sizeof(layout) / sizeof(layout[0]);
	hr = D3DX10CompileFromFile(L"simple.hlsl", NULL, NULL, "VS", "vs_4_0", dwShaderFlags, NULL, NULL, &pBlob, &err, NULL );
	if (FAILED(hr))
	{
		const char* message = (const char*)err->GetBufferPointer();
		MessageBoxA(0, message, "Error happens!", MB_OK);		
	}
	// Create the vertex shader
	hr = md3dDevice->CreateVertexShader( (DWORD*)pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &pVertexShader );

	hr = md3dDevice->CreateInputLayout( layout, numElements, pBlob->GetBufferPointer(),
		pBlob->GetBufferSize(), &pVertexLayout );
	SAFE_RELEASE(pBlob);
	SAFE_RELEASE(err);
	hr = D3DX10CompileFromFile(L"simple.hlsl", NULL, NULL, "PS", "ps_4_0", dwShaderFlags, NULL, NULL, &pBlob, &err, NULL );
	if (FAILED(hr))
	{
		const char* message = (const char*)err->GetBufferPointer();
		MessageBoxA(0, message, "Error happens!", MB_OK);
	}	
	// Create the pixel shader
	hr = md3dDevice->CreatePixelShader( (DWORD*)pBlob->GetBufferPointer(),
		pBlob->GetBufferSize(), &pPixelShader );
	SAFE_RELEASE(pBlob);
	SAFE_RELEASE(err);
}

void Axis::draw()
{
	md3dDevice->IASetPrimitiveTopology(primitiveTopology);
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	md3dDevice->IASetInputLayout( pVertexLayout );
	md3dDevice->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
	//md3dDevice->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);
	//md3dDevice->DrawIndexed(mNumFaces*3, 0, 0);
	md3dDevice->VSSetConstantBuffers( 0, 1, &mCB);
	md3dDevice->OMSetBlendState(pBlendState, 0, 0xffffffff);
	md3dDevice->RSSetState(pRasterizerState);
	md3dDevice->OMSetDepthStencilState(pDepthStencilState, 0);	

	md3dDevice->VSSetShader( pVertexShader );
	md3dDevice->GSSetShader( NULL );
	md3dDevice->PSSetShader( pPixelShader );

	md3dDevice->Draw(mNumVertices, 0);
}

void Axis::onFrameMove(D3DXMATRIX& mWorldViewProj)
{
	// Update the Constant Buffer
	CONSTANT_BUFFER* pConstData;
	mCB->Map( D3D10_MAP_WRITE_DISCARD, NULL, ( void** )&pConstData );
	pConstData->mWorldViewProj = mWorldViewProj;
	mCB->Unmap();
}
