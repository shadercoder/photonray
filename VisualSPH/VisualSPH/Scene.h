#pragma once
#include <vector>
#include <fstream>
#include "Camera.h"
#include "Particle.h"
#include "Vertexes.h"
#include <d3d9.h>
#include <d3dx9.h>
#include <d3dx9core.h>
#include <MMSystem.h>
#include <vector>
#include <fstream>
#include <string>


#pragma warning( disable : 4996 ) // disable deprecated warning 
#include <strsafe.h>
#pragma warning( default : 4996 )
// include the Direct3D Library file
#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")
// include the win system lib
#pragma comment (lib, "winmm.lib")
using namespace std;

class Scene
{
private:
	//-----------------------------------------------------------------------------
	// Global constants
	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	// Global variables
	//-----------------------------------------------------------------------------


	vector<Particle> Particles;
	string pathToFolder;
	string filePattern;
	int firstFrame;
	int lastFrame;
	int step;

public:
	//todo incapsulate
	const bool WINDOWED;
	const int SCREEN_WIDTH;
	const int SCREEN_HEIGHT;
	Camera cam;
	LPDIRECT3D9				pDirect3D; // Used to create the D3DDevice
	LPDIRECT3DDEVICE9		pDirect3DDevice; // Our rendering device

	VOID InputParticles(const int frame);
	HRESULT DrawAxes();
	VOID Render();
	HRESULT ParticleRender();
	VOID TranslateParticles();
	VOID setView();
	HRESULT TakeScreenShot(const int frame, HWND hWnd);
	Scene(void);
	// FOR GUI!
	void setParameter(string pathToFolder, string filePattern, int firstFrame, int lastFrame, int step, float camX, float camY, float camZ, float lookX, float lookY, float lookZ);
	~Scene(void);
};

