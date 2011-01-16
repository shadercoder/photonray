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
	
	 // Initialize the blend state for alpha drawing
    D3D10_BLEND_DESC StateDesc;
    ZeroMemory(&StateDesc, sizeof(D3D10_BLEND_DESC));
    StateDesc.AlphaToCoverageEnable = FALSE;
    StateDesc.BlendEnable[0] = TRUE;
    StateDesc.SrcBlend = D3D10_BLEND_SRC_ALPHA;
    StateDesc.DestBlend = D3D10_BLEND_INV_SRC_ALPHA;
    StateDesc.BlendOp = D3D10_BLEND_OP_ADD;
    StateDesc.SrcBlendAlpha = D3D10_BLEND_ZERO;
    StateDesc.DestBlendAlpha = D3D10_BLEND_ZERO;
    StateDesc.BlendOpAlpha = D3D10_BLEND_OP_ADD;
    StateDesc.RenderTargetWriteMask[0] = D3D10_COLOR_WRITE_ENABLE_ALL;
    md3dDevice->CreateBlendState(&StateDesc, &pBlendState);

	delete [] vertices;	
}

void gParticlesContainer::draw()
{
	md3dDevice->IASetPrimitiveTopology(primitiveTopology);
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	md3dDevice->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
	float blendFactor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	md3dDevice->OMSetBlendState(pBlendState, blendFactor, 0xffffffff);
	md3dDevice->Draw(mNumParticles, 0);
}

