#pragma once
#include "d3dUtil.h"
#include <string>
using namespace std;
class HUD
{
private:
	ID3DX10Font* mFont;
	wstring text;
public:
	HUD(void);
	~HUD(void);
	//TODO :
	void setText(wstring& _text);
	void init(ID3D10Device* device);
	void draw();
};

