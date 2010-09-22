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

void Camera::setView(LPDIRECT3DDEVICE9 pDirect3DDevice)
{
	D3DXMATRIX MatrixView;
	D3DXMATRIX MAtrixProjection;
	D3DXMatrixLookAtLH(&MatrixView, &pos, &look, &D3DXVECTOR3(0.0f, 1.0f, 0.0f));
	pDirect3DDevice->SetTransform(D3DTS_VIEW, &MatrixView);

	D3DXMatrixPerspectiveFovLH(&MAtrixProjection, D3DX_PI / 4, 800.0f / 600.0f, 1.0f, 100.0f);
	pDirect3DDevice->SetTransform(D3DTS_PROJECTION, &MAtrixProjection);
}
