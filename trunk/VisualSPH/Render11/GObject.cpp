#include "DXUT.h"
#include "GObject.h"


GObject::GObject(void): mVB(NULL), mIB(NULL), md3dDevice(NULL)
{
}


GObject::~GObject(void)
{
	ReleaseCOM(mVB);
	ReleaseCOM(mIB);	
}


