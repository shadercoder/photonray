#ifndef BOX_H
#define BOX_H

#include "Common\GObject.h"

class Box: public GObject
{
public:

	Box();
	~Box();

	void init(ID3D10Device* device, D3D10_PRIMITIVE_TOPOLOGY topology, D3DXVECTOR3 dimension);
	void draw();

};


#endif // BOX_H
