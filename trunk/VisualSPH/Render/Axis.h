#pragma once
#include "gobject.h"
class Axis : public GObject
{
public:
	Axis(void);
	~Axis(void);
	void init(ID3D10Device* device);
	void draw();

};

