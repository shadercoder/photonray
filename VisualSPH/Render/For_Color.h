#pragma once 
#include <fstream>
#include <iostream>
#include <algorithm>
#include "d3dUtil.h"
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
	D3DXCOLOR GetColor(const Particle* temp);
};
