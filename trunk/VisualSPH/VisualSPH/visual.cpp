#include <d3d9.h>
#include <d3dx9.h>
#include <d3dx9core.h>
#include <MMSystem.h>
#include <vector>
#include <fstream>
#include <string>

#include "Scene.h"
#include "Camera.h"
#include "Particle.h"


#pragma warning( disable : 4996 ) // disable deprecated warning 
#include <strsafe.h>
#pragma warning( default : 4996 )
// include the Direct3D Library file
#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")
// include the win system lib
#pragma comment (lib, "winmm.lib")

#define KEYDOWN(name, key) (name[key] & 0x80)

using namespace std;

DWORD FtoDw(float f)
{
     return *((DWORD*)&f);
}


	HWND hWnd;



struct ParticleAttribute
{
     D3DXVECTOR3 position;
	 float U; 
	 float V;
	 float W;
	 float Ro;
	 float P;
};




//-----------------------------------------------------------------------------
// Name: InitialDirect3D()
// Desc: Initializes Direct3D
//-----------------------------------------------------------------------------
HRESULT InitialDirect3D( HWND hWnd, Scene* scene )
{
	// Create the D3D object, which is needed to create the D3DDevice.
	if( NULL == ( scene->pDirect3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
		return E_FAIL;
	D3DDISPLAYMODE Display;
	if( FAILED(scene->pDirect3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &Display)))
	{
		return E_FAIL;
	}
	
	// Set up the structure used to create the D3DDevice. 
	D3DPRESENT_PARAMETERS Direct3DParameter;
	ZeroMemory( &Direct3DParameter, sizeof( Direct3DParameter ) );
	Direct3DParameter.Windowed = scene->WINDOWED;
	Direct3DParameter.SwapEffect = D3DSWAPEFFECT_DISCARD;
	// Anti Aliasing
	//Direct3DParameter.MultiSampleType = D3DMULTISAMPLE_2_SAMPLES;
	Direct3DParameter.BackBufferFormat = Display.Format;
	
	// Create the Direct3D device. 
	if( FAILED( scene->pDirect3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&Direct3DParameter, &scene->pDirect3DDevice ) ) )
	{
		return E_FAIL;
	}

	// Device state would normally be set here
	scene->pDirect3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	scene->pDirect3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	scene->pDirect3DDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
	// For particles
	//pDirect3DDevice->SetRenderState(D3DRS_POINTSPRITEENABLE, true);
	//pDirect3DDevice->SetRenderState(D3DRS_POINTSCALEENABLE, true);
	//pDirect3DDevice->SetRenderState(D3DRS_POINTSIZE, FtoDw(2.5f));
	//pDirect3DDevice->SetRenderState(D3DRS_POINTSIZE_MIN, FtoDw(0.2f));
	//pDirect3DDevice->SetRenderState(D3DRS_POINTSIZE_MAX, FtoDw(5.0f));

	return S_OK;
}


//HRESULT InitialObjects()
//{
//	// [2010.08.30] debug
//	//CUSTOMVERTEX Particlesm[] = 
//	//{
//	//	{1.0f, -1.0f, 0.0f, 0x00000fff, },
//	//	{-1.0f, -1.0f, 0.0f, 0xff000fff, },
//	//	{-1.0f, 1.0f, 0.0f, 0x00000fff, },
//
//	//	{-1.0f, 1.0f, 0.0f, 0x00000fff, },
//	//	{1.0f, 1.0f, 0.0f, 0xff000fff, },
//	//	{1.0f, -1.0f, 0.0f, 0x00000fff, }
//	//};	
//	//vector<CUSTOMVERTEX> Particles(Particlesm, Particlesm + sizeof(Particlesm) / sizeof(CUSTOMVERTEX));
//	
//	if(Particles.size() == 0) 
//	{
//		// Nothing to draw
//		return S_OK;
//	}	
//	if (FAILED(pDirect3DDevice->CreateVertexBuffer(Particles.size() * sizeof(CUSTOMVERTEX), 0, D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &pBufferParticles, NULL)))
//		return E_FAIL;
//	VOID* pBV;
//	if(FAILED(pBufferParticles->Lock(0, sizeof(Particles), (void**) &pBV, 0)))
//		return E_FAIL;
//	memcpy(pBV, (void*)&Particles[0], Particles.size() * sizeof(CUSTOMVERTEX));
//	pBufferParticles->Unlock();
//	return S_OK;
//}
// Set View and Projection matrix 














//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: The window's message handler
//-----------------------------------------------------------------------------
LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	switch( msg )
	{
	case WM_DESTROY:
		PostQuitMessage( 0 );
		return 0;
		
	case WM_KEYDOWN:
		// ���� ������ ������� Esc, ����������
		// ������� ���� ����������, ����������������
		// ������������ MainWindowHandle.
		if( wParam == VK_ESCAPE )
			DestroyWindow(hWnd);
		return 0;

	case WM_PAINT:
		//sc.Render();
		ValidateRect( hWnd, NULL );
		return 0;
	}

	return DefWindowProc( hWnd, msg, wParam, lParam );
}



//-----------------------------------------------------------------------------
// Name: wWinMain()
// Desc: The application's entry point
//-----------------------------------------------------------------------------
INT WINAPI wWinMain( HINSTANCE hInst, HINSTANCE, LPWSTR, INT )
{
	UNREFERENCED_PARAMETER( hInst );

	// Register the window class
	WNDCLASSEX wc =
	{
		sizeof( WNDCLASSEX ), CS_CLASSDC, MsgProc, 0L, 0L,
		GetModuleHandle( NULL ), NULL, NULL, NULL, NULL,
		"MainWindow", NULL
	};
	RegisterClassEx( &wc );

	// Create the application's window
	hWnd = CreateWindow( "MainWindow", "VisualSPH",
		WS_OVERLAPPEDWINDOW, 200, 200, 800, 600,
		NULL, NULL, wc.hInstance, NULL );
	
	Scene sc;
	// Initialize Direct3D
	if( SUCCEEDED( InitialDirect3D( hWnd, &sc ) ) )
	{

		// Show the window
		ShowWindow( hWnd, SW_SHOWDEFAULT );
		UpdateWindow( hWnd );
		// Initial
		string path;
		string pattern;

		int firstFrame, lastFrame, stepFrame;
		{
			ifstream fin("settings.txt");
			fin >> path;			
			fin >> pattern;			
			fin >> firstFrame >> lastFrame >> stepFrame;
			float cx, cy, cz;
			float lx, ly, lz;
			fin >> cx >> cy >> cz;
			//D3DXVECTOR3 pos(x, y, z);
			//sc.cam.setPosition(&pos);
			fin >> lx >> ly >> lz;
			//D3DXVECTOR3 look(x, y, z);
			//sc.cam.setLook(&look);
			sc.setParameter(path, pattern, firstFrame, lastFrame, stepFrame, cx, cy, cz, lx, ly, lz); 
			fin.close();				
		}
		
		// Enter the message loop
		sc.setView();
		
		MSG msg;
		int frame = firstFrame;
		while(TRUE)
		{
			while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			if(msg.message == WM_QUIT)
			{
				break;
			}
			sc.InputParticles(frame);
			sc.Render();			
			//sc.TakeScreenShot(frame, hWnd);			
			frame += stepFrame;
			if (frame > lastFrame)
			{
				frame = firstFrame;
			}
		}

	}
	UnregisterClass( "MainWindow", wc.hInstance );
	
	return 0;
}


