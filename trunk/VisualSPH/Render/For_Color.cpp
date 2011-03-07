#include "DXUT.h"
#include "For_Color.h"


int	For_color::InitSpectr(char* filename)
{
	FILE *input;
	fopen_s(&input, filename,"r");
	D3DXCOLOR first[10];
	D3DXCOLOR last[10];
	fscanf_s(input, "%d\n", &N);
	int i = 0;
	while ( i < N )
	{
		fscanf_s(input,"%f %f %f %f %f %f %f %f\n", &this->start[i], &this->end[i], &this->first->b, &this->first->g, &this->first->r, &this->last->b, &this->last->g, &this->last->r );
		++i;
	}
	fclose(input);
	return N;
}

D3DXCOLOR For_color::GetColor(const Particle* temp)
{
	float conformity;	
	int i = 0;
	for (; i < N; ++i)
	{
		if (temp->density >= start[i] && temp->density < end[i])
			break;
	}
	D3DXCOLOR result = BLACK;

	if (i == N)
	{
		if ( !(temp->density >= start[i] && temp->density < end[i]) )
		{
			return result;
		}
	}

	conformity = (temp->density - start[i]) / (end[i] - start[i]);	
	result.b = first[i].b + (  last[i].b - first[i].b ) * conformity ;
	result.g = first[i].g + (  last[i].g - first[i].g ) * conformity ;
	result.r = first[i].r + (  last[i].r - first[i].r ) * conformity ;
	result.a = 1.0f;
	//std::cout<<result.b;
	//std::cout<<'\t';
	//std::cout<<result.g;
	//std::cout<<'\t';
	//std::cout<<result.r<<std::endl;
	return result;
}