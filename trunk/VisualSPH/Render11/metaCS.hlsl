struct Particle
{
	float4 pos;
};

#define SIMULATION_BLOCK_SIZE 256

RWStructuredBuffer<float> VolumeRW : register( u0 );
StructuredBuffer<Particle> ParticlesRO : register( t0 );

[numthreads(SIMULATION_BLOCK_SIZE, 1, 1)]
void metaCS( uint3 Gid : SV_GroupID, uint3 DTid : SV_DispatchThreadID, uint3 GTid : SV_GroupThreadID, uint GI : SV_GroupIndex )
{
    VolumeRW[DTid.x] = 1.0f;
}
