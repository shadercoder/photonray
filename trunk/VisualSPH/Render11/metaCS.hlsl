// Enter your shader in this window
struct Voxel
{
	float val;
};

struct Particle
{
	float4 pos;
};

RWStructuredBuffer<Voxel> VolumeRW : register( u0 );
StructuredBuffer<Particle> ParticlesRO : register( t0 );

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

[numthreads(1, 1, 1)]
void metaCS(uint3 DTid : SV_DispatchThreadID )
{
	const float scale = 170.0f;
	const float metaballsSize = 17.0f;
	const float threadshold = 17.0f * 17.0f;
	float4 pos = ParticlesRO[DTid.x].pos;
	int x = (int) (pos.x * scale);
	int y = (int) (pos.y * scale);
	int z = (int) (pos.z * scale);
	
	for (int dz = (int) -metaballsSize; dz <= (int) metaballsSize; ++dz)
	{
		
		for (int dx = (int) -metaballsSize; dx <= (int) metaballsSize; ++dx)
		{
			
			for (int dy = (int) -metaballsSize; dy <= (int) metaballsSize; ++dy)
			{
				float3 cell = float3((float) (x + dx), (float) (y + dy), (float) (z + dz));				
				//InterlockedAdd(VolumeRW[arrayIndexFromCoordinate(float3(x + dx, y + dy, z + dz))].val, (int)calcMetaball(pos * scale, cell, threadshold) * 1000);				
				//VolumeRW[arrayIndexFromCoordinate(cell)].val = VolumeRW[arrayIndexFromCoordinate(cell)].val + calcMetaball(pos * scale, cell, threadshold) * isInside(x + dx, y + dy, z + dz);
				VolumeRW[arrayIndexFromCoordinate(float3(x + dx, y + dy, z + dz))].val = 10000;
			}
		}
	}			    
}
