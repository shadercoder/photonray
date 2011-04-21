#include "DXUT.h"
#include "Axis.h"


Axis::Axis(void)
{
}


Axis::~Axis(void)
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

void Axis::init(ID3D11Device* device, ID3D11DeviceContext* md3dContext)
{
	md3dDevice = device;
	this->md3dContext = md3dContext;
	mNumVertices = 6;
	primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;

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

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex) * mNumVertices;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
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

		// Create a blend state to disable alpha blending
	D3D11_BLEND_DESC BlendState;
	ZeroMemory( &BlendState, sizeof( D3D11_BLEND_DESC ) );
	BlendState.RenderTarget[0].BlendEnable = FALSE;
	BlendState.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
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
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,  D3D11_INPUT_PER_VERTEX_DATA, 0 },		
	};
	int numElements = sizeof(layout) / sizeof(layout[0]);
	hr = D3DX11CompileFromFile(L"simple.hlsl", NULL, NULL, "VS", "vs_4_0", dwShaderFlags, NULL, NULL, &pBlob, &err, NULL );
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
	hr = D3DX11CompileFromFile(L"simple.hlsl", NULL, NULL, "PS", "ps_4_0", dwShaderFlags, NULL, NULL, &pBlob, &err, NULL );
	if (FAILED(hr))
	{
		const char* message = (const char*)err->GetBufferPointer();
		MessageBoxA(0, message, "Error happens!", MB_OK);
	}	
	// Create the pixel shader
	hr = md3dDevice->CreatePixelShader( (DWORD*)pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &pPixelShader );
	SAFE_RELEASE(pBlob);
	SAFE_RELEASE(err);

	// Labels init
	xLabel.init(md3dDevice, md3dContext, L"X.dds");
	xLabel.setPosition(1, 0, 0, 0.1, 0.1);
	yLabel.init(md3dDevice, md3dContext, L"Y.dds");
	yLabel.setPosition(0, 1, 0, 0.1, 0.1);
	zLabel.init(md3dDevice, md3dContext, L"Z.dds");
	zLabel.setPosition(0, 0, 1, 0.1, 0.1);
}

void Axis::draw()
{
	md3dContext->IASetPrimitiveTopology(primitiveTopology);
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	md3dContext->IASetInputLayout( pVertexLayout );
	md3dContext->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
	//md3dDevice->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);
	//md3dDevice->DrawIndexed(mNumFaces*3, 0, 0);
	md3dContext->VSSetConstantBuffers( 0, 1, &mCB);
	md3dContext->OMSetBlendState(pBlendState, 0, 0xffffffff);
	md3dContext->RSSetState(pRasterizerState);
	md3dContext->OMSetDepthStencilState(pDepthStencilState, 0);	

	md3dContext->VSSetShader( pVertexShader, NULL, 0);
	md3dContext->GSSetShader( NULL, NULL, 0);
	md3dContext->PSSetShader( pPixelShader, NULL, 0);

	md3dContext->Draw(mNumVertices, 0);
	xLabel.draw();
	yLabel.draw();
	zLabel.draw();
}

void Axis::onFrameMove(D3DXMATRIX& mWorldViewProj, D3DXMATRIX View)
{
	// Update the Constant Buffer
	CONSTANT_BUFFER* pConstData;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	md3dContext->Map(mCB, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mappedResource);
	pConstData = (CONSTANT_BUFFER*) mappedResource.pData;
	pConstData->mWorldViewProj = mWorldViewProj;
	md3dContext->Unmap(mCB, 0);
	xLabel.onFrameMove(mWorldViewProj, View);
	yLabel.onFrameMove(mWorldViewProj, View);
	zLabel.onFrameMove(mWorldViewProj, View);
}
