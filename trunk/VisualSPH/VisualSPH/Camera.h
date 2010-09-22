#pragma once
#include <d3d9.h>
#include <d3dx9.h>

class Camera
{
private:     
	D3DXVECTOR3 right;
	D3DXVECTOR3 up;
	D3DXVECTOR3 look;
	D3DXVECTOR3 pos;
public:     

	Camera();     
	~Camera();

	void strafe(float units); // left / right
	void fly(float units);    // up / down
	void walk(float units);   // forward / back

	void pitch(float angle); // rotation around the vector right
	void yaw(float angle);   // rotation around the vector up
	void roll(float angle);  // rotation around the vector eye

	void setView(LPDIRECT3DDEVICE9 pDirect3DDevice);

	void getViewMatrix(D3DXMATRIX* V);     
	void getPosition(D3DXVECTOR3* pos);
	void setPosition(D3DXVECTOR3* pos);
	void getRight(D3DXVECTOR3* right);
	void getUp(D3DXVECTOR3* up);
	void getLook(D3DXVECTOR3* look);
	void setLook(D3DXVECTOR3* look);
};
