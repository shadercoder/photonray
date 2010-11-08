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

Camera TheCamera(Camera::AIRCRAFT);
//LPDIRECT3DDEVICE9 pDirect3DDevice;
Scene sc;
HINSTANCE hInstance;

// Функция обработки нажатия клавиш  
bool EnterKey(float timeDelta)
{
	if(sc.pDirect3DDevice)
	{
		if (GetAsyncKeyState('W') & 0x8000f)
			TheCamera.walk(4.0f * timeDelta);

		if (GetAsyncKeyState('S') & 0x8000f)
			TheCamera.walk(-4.0f * timeDelta);

		if (GetAsyncKeyState('A') & 0x8000f)
			TheCamera.strafe(-4.0f * timeDelta);

		if (GetAsyncKeyState('D') & 0x8000f)
			TheCamera.strafe(4.0f * timeDelta);

		if (GetAsyncKeyState('R') & 0x8000f)
			TheCamera.fly(4.0f * timeDelta);

		if (GetAsyncKeyState('F') & 0x8000f)
			TheCamera.fly(-4.0f * timeDelta);

		if (GetAsyncKeyState(VK_UP) & 0x8000f)
			TheCamera.pitch(-1.0f * timeDelta);

		if (GetAsyncKeyState(VK_DOWN) & 0x8000f)
			TheCamera.pitch(1.0f * timeDelta);

		if (GetAsyncKeyState(VK_LEFT) & 0x8000f)
			TheCamera.yaw(-1.0f * timeDelta);
			
		if (GetAsyncKeyState(VK_RIGHT) & 0x8000f)
			TheCamera.yaw(1.0f * timeDelta);

		if (GetAsyncKeyState('N') & 0x8000f)
			TheCamera.roll(1.0f * timeDelta);

		if (GetAsyncKeyState('M') & 0x8000f)
			TheCamera.roll(-1.0f * timeDelta);
		
		// Обновление матрицы вида согласно новому
        //местоположению и ориентации камеры
		D3DXMATRIX V;
	//	D3DXMatrixIdentity(&V);
		TheCamera.getViewMatrix(&V);
		sc.pDirect3DDevice->SetTransform(D3DTS_VIEW, &V);
	//	TheCamera.setView(sc.pDirect3DDevice);
	}
	return true;
}

void EnterMsgLoop (int firstFrame, int lastFrame, int stepFrame, bool (*Display)(float timeDelta))
{
	int frame = firstFrame;
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));

	static float lastTime = (float)timeGetTime(); 

	while(msg.message != WM_QUIT)
	{
		if(PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			if(msg.message == WM_QUIT)
			{
				break;
			}
		}
		else
        {
			float currTime  = (float)timeGetTime();
			float timeDelta = (currTime - lastTime)*0.001f;

			Display(timeDelta);
			if (GetAsyncKeyState('P') & 0x8000f)
			{
				sc.TakeScreenShot(frame);
			}
			sc.InputParticles(frame);
			sc.Render();
			lastTime = currTime;
 		}

		frame += stepFrame;
		if (frame > lastFrame)
		{
			frame = firstFrame;
		}
	}
}

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
	sc.cam = ::TheCamera;
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
		
		//MSG msg;
		int frame = firstFrame;
		/*while(TRUE)
		{
			while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			if(msg.message == WM_QUIT)
			{
				break;
			}*/
			sc.InputParticles(frame);
			sc.Render();
			EnterMsgLoop(firstFrame, lastFrame, stepFrame, EnterKey);
			//sc.TakeScreenShot(frame);	
			/*frame += stepFrame;
			if (frame > lastFrame)
			{
				frame = firstFrame;
			}
		}*/

	}
	UnregisterClass( "MainWindow", wc.hInstance );
	
	return 0;
}



/*
int WINAPI WinMain(HINSTANCE hInst,	HINSTANCE hprevinstance, LPSTR lpcmdline, int ncmdshow)
{
    WNDCLASSEX windowsclass;
    HWND hwnd;
    MSG msg;
    hInstance = hInst;

    windowsclass.cbSize = sizeof(WNDCLASSEX);
    windowsclass.style = CS_DBLCLKS|CS_OWNDC|CS_HREDRAW|CS_VREDRAW;
    windowsclass.lpfnWndProc = MsgProc;
    windowsclass.cbClsExtra = 0;
    windowsclass.cbWndExtra = 0;
    windowsclass.hInstance = hInst;
    windowsclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    windowsclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    windowsclass.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
    windowsclass.lpszMenuName = NULL;
    windowsclass.lpszClassName = "WINDOWSCLASS";
    windowsclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&windowsclass))
        return 0;

    hwnd = CreateWindowEx(NULL, "WINDOWSCLASS", "Загрузка Х-файла с вращением", WS_OVERLAPPEDWINDOW|WS_VISIBLE,
        0, 0, 770, 500, NULL, NULL, hInst, NULL);
    if (!hwnd)
        return 0;

    if (SUCCEEDED(InitialDirect3D(hwnd,&sc)))
    {
        ShowWindow(hwnd, SW_SHOWDEFAULT);
        UpdateWindow(hwnd);
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
			int frame = firstFrame;
		while (true)
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
    return (int)msg.wParam;
}*/
