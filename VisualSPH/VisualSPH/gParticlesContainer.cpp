#include "gParticlesContainer.h"
#include <vector>

gParticlesContainer::gParticlesContainer(void):
MAX_PARTICLES(1 << 16)
{
	//vertices = new Vertex[MAX_PARTICLES];
}


gParticlesContainer::~gParticlesContainer(void)
{
}

void gParticlesContainer::loadData(Particle* p, int particleCount)
{
	mNumParticles = particleCount;
	Vertex* vertices = NULL;
	mVB->Map(D3D10_MAP_WRITE_DISCARD, 0, (void**) &vertices);
	for (int i = 0; i < particleCount; ++i)
	{
		vertices[i].pos = p[i].position;
		vertices[i].color = BLUE;
	}
	mVB->Unmap();
}

void gParticlesContainer::init(ID3D10Device* device)
{
	md3dDevice = device;

	primitiveTopology = D3D10_PRIMITIVE_TOPOLOGY_POINTLIST;

	mNumParticles = MAX_PARTICLES;
	mNumFaces = 0;
	//Create vertex buffer
	Vertex* vertices = new Vertex[mNumParticles];
	
	D3D10_BUFFER_DESC vbd;
	vbd.Usage = D3D10_USAGE_DYNAMIC;
	vbd.ByteWidth = sizeof(Vertex) * mNumParticles;
	vbd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	vbd.MiscFlags = 0;
	
	HR(md3dDevice->CreateBuffer(&vbd, NULL, &mVB));
	
	delete [] vertices;	
}

void gParticlesContainer::draw()
{
	md3dDevice->IASetPrimitiveTopology(primitiveTopology);
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	md3dDevice->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
	md3dDevice->Draw(mNumParticles, 0);
}

