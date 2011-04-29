// Enter your shader in this window
struct Voxel
{
	float val;
};

#define SIMULATION_BLOCK_SIZE 256

cbuffer cbGrid : register(b0)
{
	uint numParticles;
};

struct Particle
{
	float4 pos;
};

struct Item
{
	uint key;
	uint val;
};

RWStructuredBuffer<Item> GridKeysRW : register( u0 );
RWStructuredBuffer<Particle> ParticlesRW : register( u0 );
StructuredBuffer<Particle> ParticlesRO : register( t0 );

RWStructuredBuffer<unsigned int> GridRW : register( u0 );
StructuredBuffer<Item> GridRO : register( t1 );

RWStructuredBuffer<uint2> GridIndicesRW : register( u0 );
StructuredBuffer<uint2> GridIndicesRO : register( t2 );


int arrayIndexFromCoordinate(float3 pos)
{
	int i = pos.x;
	int j = pos.y;
	int k = pos.z;
	return (i * 256 + j) * 256 + k;
}

float calcMetaball(float3 centerBall, float3 cell, const float threadshold)
{	
	float3 tmp = centerBall - cell;	
	//float len = pow(tmp.x, 2) + pow(tmp.y, 2) + pow(tmp.z, 2);
	float len = dot(tmp, tmp);
	if (len > threadshold) {
		return 0.0f;
	}
	float t = threadshold / (len + 1e-5f);
	float res = pow(t, 4.0f);	
	return res;
}

int isInside(int i, int j, int k)
{
	if (i >= 0 && j >= 0 && k >= 0 && i < 256 && j < 256 && k < 256)
	{
		return 1;
	}
	return 0;
}

unsigned int GridGetKey(Item keyvaluepair)
{
    return keyvaluepair.key;
}

unsigned int GridGetValue(Item keyvaluepair)
{
    return (keyvaluepair.val);
}

uint cellIndex(float4 pos)
{
	int hx = 1;
	int hy = 1;
	int hz = 1;
	uint res = (((pos.x / hx) * 256 + pos.y / hy) * 256 + pos.z / hz);
	return res;
}

[numthreads(1, 1, 1)]
void BuildGridCS(uint3 DTid : SV_DispatchThreadID )
{
	float4 pos = ParticlesRO[DTid.x].pos;
	GridKeysRW[DTid.x].key = cellIndex(pos);
	GridKeysRW[DTid.x].val = DTid.x;
}

[numthreads(SIMULATION_BLOCK_SIZE, 1, 1)]
void BuildGridIndicesCS( uint3 Gid : SV_GroupID, uint3 DTid : SV_DispatchThreadID, uint3 GTid : SV_GroupThreadID, uint GI : SV_GroupIndex )
{
    const unsigned int G_ID = DTid.x; // Grid ID to operate on
    unsigned int G_ID_PREV = (G_ID == 0)? numParticles : G_ID; G_ID_PREV--;
    unsigned int G_ID_NEXT = G_ID + 1; if (G_ID_NEXT == numParticles) { G_ID_NEXT = 0; }
    
    unsigned int cell = GridGetKey( GridRO[G_ID] );
    unsigned int cell_prev = GridGetKey( GridRO[G_ID_PREV] );
    unsigned int cell_next = GridGetKey( GridRO[G_ID_NEXT] );
    if (cell != cell_prev)
    {
        // I'm the start of a cell
        GridIndicesRW[cell].x = G_ID;
    }
    if (cell != cell_next)
    {
        // I'm the end of a cell
        GridIndicesRW[cell].y = G_ID + 1;
    }
}

//--------------------------------------------------------------------------------------
// Rearrange Particles
//--------------------------------------------------------------------------------------

[numthreads(SIMULATION_BLOCK_SIZE, 1, 1)]
void RearrangeParticlesCS( uint3 Gid : SV_GroupID, uint3 DTid : SV_DispatchThreadID, uint3 GTid : SV_GroupThreadID, uint GI : SV_GroupIndex )
{
    const unsigned int ID = DTid.x; // Particle ID to operate on
    const unsigned int G_ID = GridGetValue( GridRO[ ID ] );
    ParticlesRW[ID] = ParticlesRO[ G_ID ];
}


[numthreads(1, 1, 1)]
void metaCS(uint3 DTid : SV_DispatchThreadID )
{
		    
}
