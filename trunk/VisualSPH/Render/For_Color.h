#ifndef __FOR_COLOR__
#define __FOR_COLOR__

#include <fstream>
#include <iostream>
#include <algorithm>
#include <d3d10.h>
#include <D3DX10.h>
#include "Particle.h"


class For_color
{
	float start[10];
	float end[10];
	D3DXCOLOR first[10],last[10];
	int N;
public:
	For_color() 
	{
		for (int i(0);i<10;++i)
		{
			start[i] = 0 ;
			end[i] = 0;
			first[i].b = 0;
			first[i].g = 0;
			first[i].r = 0;

			last[i].b = 0;
			last[i].g = 0;
			last[i].r = 0;
		}
		N = 0; 
	}
	int	InitSpectr(char*);
	D3DXCOLOR GetColor(Particle& temp);
};

#endif