#include <d3d9.h>
#include "d3dx9.h"
#include "d3dx9core.h"
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

//Main window
HWND hWnd;



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
	scene->pDirect3DDevice->SetRenderState(D3DRS_POINTSIZE, FtoDw(2.5f));
	//pDirect3DDevice->SetRenderState(D3DRS_POINTSIZE_MIN, FtoDw(0.2f));
	//pDirect3DDevice->SetRenderState(D3DRS_POINTSIZE_MAX, FtoDw(5.0f));

	return S_OK;
}


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
		// Если нажата клавиша Esc, уничтожаем
		// главное окно приложения, идентифицируемое
		// дескриптором MainWindowHandle.
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
		WS_OVERLAPPEDWINDOW, 100, 100, 800, 600,
		NULL, NULL, wc.hInstance, NULL );
	
	Scene sc;

	// Initialize Direct3D
	if( SUCCEEDED( InitialDirect3D( hWnd, &sc ) ) )
	{

		// Show the window
		ShowWindow( hWnd, SW_SHOWDEFAULT );
		UpdateWindow( hWnd );
		sc.setHWND(hWnd);
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
			fin >> lx >> ly >> lz;
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
			//sc.TakeScreenShot(frame);	
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



