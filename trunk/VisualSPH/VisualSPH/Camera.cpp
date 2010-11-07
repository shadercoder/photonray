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

void Camera::getViewMatrix(D3DXMATRIX* V)
{   

   	D3DXVec3Normalize(&look, &look);
   
	D3DXVec3Cross(&up, &look, &right);
    
	D3DXVec3Normalize(&up, &up);
	
	D3DXVec3Cross(&right, &up, &look);
    
	D3DXVec3Normalize(&right, &right);


	float x = -D3DXVec3Dot(&pos,&right);
    float y = -D3DXVec3Dot(&pos,&up);
    float z = -D3DXVec3Dot(&pos,&look);

	V->_11 = right.x;
	V->_12 = up.x;
	V->_13 = look.x;
	V->_14 = 0;

	V->_21 = right.y;
	V->_22 = up.y;
	V->_23 = look.y;
	V->_24 = 0;

	V->_31 = right.z;
	V->_32 = up.z;
	V->_33 = look.z;
	V->_34 = 0;

	V->_41 = x;
	V->_42 = y;
	V->_43 = z;
	V->_44 = 1.0f;
}

void Camera::pitch(float angle)
{
     D3DXMATRIX T;
     D3DXMatrixRotationAxis(&T, &right, angle);

	 //Transform coordinates

	 D3DXVec3TransformCoord(&up,&up,&T);
	 D3DXVec3TransformCoord(&look,&look,&T);
}

void Camera::yaw(float angle)
{
     D3DXMATRIX T;

     // Для наземных объектов выполняем вращение
     // вокруг мировой оси Y (0, 1, 0)
     if(cameraType == LANDOBJECT)
          D3DXMatrixRotationY(&T, angle);

     // Для летающих объектов выполняем вращение
     // относительно верхнего вектора
     if(cameraType == AIRCRAFT)
          D3DXMatrixRotationAxis(&T, &up, angle);

     // Поворот векторов _right и _look относительно
     // вектора _up или оси Y
     D3DXVec3TransformCoord(&right, &right, &T);
     D3DXVec3TransformCoord(&look, &look, &T);
}

void Camera::roll(float angle)
{
     // Вращение только для летающих объектов
     if(cameraType == AIRCRAFT)
     {
          D3DXMATRIX T;
          D3DXMatrixRotationAxis(&T, &look, angle);

          // Поворот векторов _up и _right относительно
          // вектора _look
          D3DXVec3TransformCoord(&right, &right, &T);
          D3DXVec3TransformCoord(&up, &up, &T);
     }
}

void Camera::strafe(float units)
{
     // Для наземных объектов перемещение только в плоскости xz
     if(cameraType == LANDOBJECT)
	 {
          pos += D3DXVECTOR3(right.x, 0.0f, 0.0f) * units;
	 }
     if(cameraType == AIRCRAFT)
          pos += right * units;
}

void Camera::walk(float units)
{
     // Для наземных объектов перемещение только в плоскости xz
     if(cameraType == LANDOBJECT)
	 {
          pos += D3DXVECTOR3(look.x, 0.0f, look.z) * units;
	 }

     if(cameraType == AIRCRAFT)
          pos += look * units;
}

void Camera::fly(float units)
{
     if(cameraType == AIRCRAFT)
     {
		 pos += up * units; 
	 }		  
}

Camera::Camera(CameraType cameraType)
{
	this->cameraType = cameraType;

	this->pos   = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	this->look  = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	this->right = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	this->up    = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
}