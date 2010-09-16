#include "Camera.h"


Camera::Camera(void)
{
}


Camera::~Camera(void)
{
}

void Camera::getPosition(D3DXVECTOR3* pos)
{
	*pos = this->pos;
}

void Camera::setPosition(D3DXVECTOR3* pos)
{
	this->pos = D3DXVECTOR3(*pos);
}

void Camera::getLook(D3DXVECTOR3* look)
{
	*look = this->look;
}

void Camera::setLook(D3DXVECTOR3* look)
{
	this->look = D3DXVECTOR3(*look);
}
