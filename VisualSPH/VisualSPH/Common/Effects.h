#pragma once
#include "d3dUtil.h"
#include <string>

namespace fx
{
	extern ID3D10Effect* SkyFX;
	extern ID3D10Effect* TerrainFX;
	extern ID3D10Effect* MeshFX;
	extern ID3D10Effect* BuildShadowMapFX;
	extern ID3D10Effect* FireFX;
	extern ID3D10Effect* RainFX;
 
	void InitAll(ID3D10Device* device);
	void DestroyAll();
};
