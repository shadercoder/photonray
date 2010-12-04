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
		{D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), RED},
		{D3DXVECTOR3(1.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), RED},
		{D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), GREEN},
		{D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), GREEN},
		{D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), BLUE},
		{D3DXVECTOR3(0.0f, 0.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), BLUE},
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
}

void Axis::draw()
{
	md3dDevice->IASetPrimitiveTopology(primitiveTopology);
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	md3dDevice->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
	//md3dDevice->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);
	//md3dDevice->DrawIndexed(mNumFaces*3, 0, 0);
	md3dDevice->Draw(mNumVertices, 0);
}
