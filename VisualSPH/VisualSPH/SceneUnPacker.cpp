#pragma once
#include "SceneConstraints.h"
#include "SceneUnPacker.h"

SceneUnPacker::SceneUnPacker(char fn[])
{
	pIn = fopen(fn, "rb");
	curr_frame = 0;
	fread(&frames, sizeof(int), 1, pIn);
//	printf("There are %d frames.\n", frames);
}

// ����������� � buff[] = { x, y, z, u, v, w, Ro, �� � ��������� ��������};
bool SceneUnPacker::getNextParticles()
{
	if(curr_frame == frames)
		return false;
	
	int n; fread(&n, sizeof(int), 1, pIn);
	
	for(int i = 0; i < n; ++i)
		fread(&buff, sizeof(float), DIMENSIONS, pIn);
	
	++curr_frame;
	return true;
}