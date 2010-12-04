#include "Box.h"

Box::Box()
{
	
}

Box::~Box()
{
	ReleaseCOM(mVB);
	ReleaseCOM(mIB);
}

void Box::init(ID3D10Device* device, D3D10_PRIMITIVE_TOPOLOGY topology, D3DXVECTOR3 dimension)
 {
	md3dDevice = device;
	primitiveTopology = topology;
	mNumVertices = 8;
	mNumFaces    = 12; // 2 per quad
	float x_max = dimension.x / 2.f;  
	float x_min = -dimension.x / 2.f;
	float y_max = dimension.y / 2.f;  
	float y_min = -dimension.y / 2.f;
	float z_max = dimension.z / 2.f;  
	float z_min = -dimension.z / 2.f;
	// Create vertex buffer
	Vertex vertices[] =
	{
		{D3DXVECTOR3(x_min, y_min, z_min), D3DXVECTOR3(0.0f, 0.0f, 0.0f), BLACK},
		{D3DXVECTOR3(x_min, y_max, z_min), D3DXVECTOR3(0.0f, 0.0f, 0.0f), BLACK},
		{D3DXVECTOR3(x_max, y_max, z_min), D3DXVECTOR3(0.0f, 0.0f, 0.0f), BLACK},
		{D3DXVECTOR3(x_max, y_min, z_min), D3DXVECTOR3(0.0f, 0.0f, 0.0f), BLACK},

		{D3DXVECTOR3(x_min, y_min, z_max), D3DXVECTOR3(0.0f, 0.0f, 0.0f), BLACK},
		{D3DXVECTOR3(x_min, y_max, z_max), D3DXVECTOR3(0.0f, 0.0f, 0.0f), BLACK},
		{D3DXVECTOR3(x_max, y_max, z_max), D3DXVECTOR3(0.0f, 0.0f, 0.0f), BLACK},
		{D3DXVECTOR3(x_max, y_min, z_max), D3DXVECTOR3(0.0f, 0.0f, 0.0f), BLACK},
	};

	// Scale the box.
	//for(DWORD i = 0; i < mNumVertices; ++i)
	//	vertices[i].pos *= scale;


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

	DWORD indices[] = {
		// front face
		0 , 1 ,
		1 , 2 ,
		2 , 3 ,
		3 , 0 ,

		0 , 4 ,
		4 , 5 ,
		5 , 6 ,
		6 , 7,
		7 , 4 ,

		5 , 1 ,
		6 , 2 ,
		7 , 3 ,
	};

	D3D10_BUFFER_DESC ibd;
	ibd.Usage = D3D10_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(DWORD) * D3D10_PRIMITIVE_TOPOLOGY_LINELIST*12;
	ibd.BindFlags = D3D10_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	D3D10_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = indices;
	HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mIB));
}

void Box::draw()
{
	md3dDevice->IASetPrimitiveTopology(primitiveTopology);
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	md3dDevice->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
	md3dDevice->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);
	md3dDevice->DrawIndexed(D3D10_PRIMITIVE_TOPOLOGY_LINELIST*12, 0, 0);
}
