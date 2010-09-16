#include <d3d9.h>
#include <d3dx9.h>
#include <d3dx9core.h>
#include <MMSystem.h>
#include <vector>
#include <fstream>
#include <string>
#include "Camera.h"
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

//-----------------------------------------------------------------------------
// Global constants
//-----------------------------------------------------------------------------
const bool WINDOWED = TRUE;
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;


//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
HWND hWnd;
LPDIRECT3D9				pDirect3D = NULL; // Used to create the D3DDevice
LPDIRECT3DDEVICE9		pDirect3DDevice = NULL; // Our rendering device


struct CUSTOMVERTEX
{
	FLOAT x,y,z;
	DWORD color;
};
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE)

struct Particle
{
	D3DXVECTOR3 position;
	D3DCOLOR color;
};
#define D3DFVF_PARTICLE (D3DFVF_XYZ | D3DFVF_DIFFUSE)

struct ParticleAttribute
{
     D3DXVECTOR3 position;
	 float U; 
	 float V;
	 float W;
	 float Ro;
	 float P;
};

vector<Particle> Particles;



//-----------------------------------------------------------------------------
// Name: InitialDirect3D()
// Desc: Initializes Direct3D
//-----------------------------------------------------------------------------
HRESULT InitialDirect3D( HWND hWnd )
{
	// Create the D3D object, which is needed to create the D3DDevice.
	if( NULL == ( pDirect3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
		return E_FAIL;
	D3DDISPLAYMODE Display;
	if( FAILED(pDirect3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &Display)))
	{
		return E_FAIL;
	}
	
	// Set up the structure used to create the D3DDevice. 
	D3DPRESENT_PARAMETERS Direct3DParameter;
	ZeroMemory( &Direct3DParameter, sizeof( Direct3DParameter ) );
	Direct3DParameter.Windowed = WINDOWED;
	Direct3DParameter.SwapEffect = D3DSWAPEFFECT_DISCARD;
	// Anti Aliasing
	//Direct3DParameter.MultiSampleType = D3DMULTISAMPLE_2_SAMPLES;
	Direct3DParameter.BackBufferFormat = Display.Format;
	
	// Create the Direct3D device. 
	if( FAILED( pDirect3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&Direct3DParameter, &pDirect3DDevice ) ) )
	{
		return E_FAIL;
	}

	// Device state would normally be set here
	pDirect3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	pDirect3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	pDirect3DDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
	// For particles
	//pDirect3DDevice->SetRenderState(D3DRS_POINTSPRITEENABLE, true);
	//pDirect3DDevice->SetRenderState(D3DRS_POINTSCALEENABLE, true);
	//pDirect3DDevice->SetRenderState(D3DRS_POINTSIZE, FtoDw(2.5f));
	//pDirect3DDevice->SetRenderState(D3DRS_POINTSIZE_MIN, FtoDw(0.2f));
	//pDirect3DDevice->SetRenderState(D3DRS_POINTSIZE_MAX, FtoDw(5.0f));

	return S_OK;
}

VOID InputParticles(const int frame, string path, string pattern)
{
	Particles.clear();
	//string filename = "./frames/2/";
	string filename = path;
	char numfile[5] = {};
	sprintf(numfile, "%d", frame);
	for (int i = 0; i < 5 - strlen(numfile); ++i)
	{
		filename.append("0");
	}
	filename.append(numfile);
	//filename.append("_flu.txt");
	filename.append(pattern);
	fstream fin(filename);
	// TODO: skip header
	{
		string tmp;
		for (int i = 0; i < 8; ++i) 
		{
			fin >> tmp;
		}
	}
	while (!fin.eof())
	{
		float x, y, z;
		Particle current;
		fin >> x >> y >> z;
		current.position.x = x;
		current.position.y = y;
		current.position.z = z;
		current.color = D3DCOLOR_ARGB(255, 255, 255, 255);
		// TODO: unused parameters
		for (int j = 0; j < 5; ++j) 
		{
			float tmp;
			fin >> tmp;
		}
		Particles.push_back(current);
	}
	fin.close();
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
VOID InitialMatrix(Camera cam)
{
	D3DXMATRIX MatrixView;
	D3DXMATRIX MAtrixProjection;
	D3DXVECTOR3 pos, look;
	cam.getPosition(&pos);
	cam.getLook(&look);
	D3DXMatrixLookAtLH(&MatrixView, &pos, &look, &D3DXVECTOR3(0.0f, 1.0f, 0.0f));
	pDirect3DDevice->SetTransform(D3DTS_VIEW, &MatrixView);

	D3DXMatrixPerspectiveFovLH(&MAtrixProjection, D3DX_PI / 4, 800.0f / 600.0f, 1.0f, 100.0f);
	pDirect3DDevice->SetTransform(D3DTS_PROJECTION, &MAtrixProjection);
}

VOID Matrix()
{
	D3DXMATRIX MatrixWorld;
	D3DXMATRIX MatrixWorldTranslate;
	D3DXMATRIX MatrixWorldRotate;
	D3DXMATRIX MatrixWorldScale;
	D3DXMatrixTranslation(&MatrixWorldTranslate, 0.0f, 0.0f, 0.0f);
	// conver from local coord to presenter's
	D3DXMatrixRotationX(&MatrixWorldRotate, -D3DX_PI / 2.0f);
	D3DXMatrixScaling(&MatrixWorldScale, 2.0f, 2.0f, 2.0f);
	MatrixWorld = MatrixWorldRotate * MatrixWorldTranslate * MatrixWorldScale;
	pDirect3DDevice->SetTransform(D3DTS_WORLD, &MatrixWorld);
}

//-----------------------------------------------------------------------------
// Name: Cleanup()
// Desc: Releases all previously initialized objects
//-----------------------------------------------------------------------------
VOID Cleanup()
{

	if( pDirect3DDevice != NULL )
		pDirect3DDevice->Release();

	if( pDirect3D != NULL )
		pDirect3D->Release();
}

// Particle render
HRESULT ParticleRender()
{	
	LPDIRECT3DVERTEXBUFFER9 pBufferParticles = NULL;
	if(Particles.size() == 0) 
	{
		// Nothing to draw
		return S_OK;
	}	
	// Setup vertex buffer for available particles
	if (FAILED(pDirect3DDevice->CreateVertexBuffer(Particles.size() * sizeof(Particle), 0, D3DFVF_PARTICLE, D3DPOOL_DEFAULT, &pBufferParticles, NULL)))
		return E_FAIL;
	VOID* pBV;
	if(FAILED(pBufferParticles->Lock(0, sizeof(Particles), (void**) &pBV, 0)))
		return E_FAIL;
	// Copy particles to vertex buffer
	memcpy(pBV, (void*)&Particles[0], Particles.size() * sizeof(Particle));
	pBufferParticles->Unlock();
	// Render
	pDirect3DDevice->SetStreamSource(0, pBufferParticles, 0, sizeof(Particle));
	pDirect3DDevice->SetFVF(D3DFVF_PARTICLE);
	pDirect3DDevice->DrawPrimitive(D3DPT_POINTLIST, 0, Particles.size());
	pBufferParticles->Release();
	return S_OK;
}


HRESULT TakeScreenShot(const int frame)
{    
	HRESULT hr = S_OK;    
	LPDIRECT3DSURFACE9 frontbuf;    
	char filename[64] = {};    
	FILE* f;    
	int x, y;    
	RECT rcWindow;     

	sprintf(filename, "./screenshots/frame_%d.bmp", frame);        
	

	// get the screen resolution. If it's a windowed application get the whole    
	// screen size. If it's a fullscreen application you might have somewhere     
	// your defines as: #define SCREEN_WIDTH 800    
	if( !WINDOWED )    
	{        
		x = SCREEN_WIDTH;        
		y = SCREEN_HEIGHT;    
	}    
	else    
	{        
		x = GetSystemMetrics( SM_CXSCREEN );        
		y = GetSystemMetrics( SM_CYSCREEN );                
		// to get the window sizes        
		GetWindowRect( hWnd, &rcWindow );    
	}     
	// here we create an empty Surface. The parameter D3DFMT_A8R8G8B8 creates an 32 bit image with    
	// an alpha channel and 8 bits per channel.    
	if( FAILED( hr = pDirect3DDevice->CreateOffscreenPlainSurface(x, y, D3DFMT_A8R8G8B8, D3DPOOL_SCRATCH, &frontbuf, NULL)))        
		return hr;     
	// now we copy the entire frontbuffer into our new surface. The first parameter is NULL since    
	// we assume we have only one swap chain    
	if( FAILED( hr = pDirect3DDevice->GetFrontBufferData(NULL, frontbuf)))    
	{        
		// if this fails release our surface so we have no memory leak        
		frontbuf->Release();        
		return hr;    
	}     
	// This is the most important functions. The DirectX-SDK provides this handy little function to    
	// save our surface to a file. The first parameter is our specified filename, the second parameter    
	// tells DirectX what kind of file we want to save (in this example we decide to save to BMP)    
	// Note the difference between a fullscreen screenshot and a windowed one. If we have a windowed application    
	// we only want the specified RECT saved from our screen capture    
	if( !WINDOWED )        
		D3DXSaveSurfaceToFile( filename, D3DXIFF_BMP, frontbuf, NULL, NULL);    
	else        
		D3DXSaveSurfaceToFile( filename, D3DXIFF_BMP, frontbuf, NULL, &rcWindow);     

	frontbuf->Release();     
	return hr;
}


//-----------------------------------------------------------------------------
// Name: DrawAxes()
// Desc: Draws the axes x, y, z
//-----------------------------------------------------------------------------
HRESULT DrawAxes()
{	
	// create vertexes and buffers for axes
	LPDIRECT3DVERTEXBUFFER9 pBufferVertexAxes	= NULL;
	LPDIRECT3DINDEXBUFFER9 pBufferIndex			= NULL;
	CUSTOMVERTEX Vertexes[] = 
	{
		{0.0f, 0.0f, 0.0f, D3DCOLOR_ARGB(255, 255, 255, 255), },
		{1.0f, 0.0f, 0.0f, D3DCOLOR_ARGB(255, 255, 0, 0), },
		{0.0f, 1.0f, 0.0f, D3DCOLOR_ARGB(255, 0, 255, 0), },
		{0.0f, 0.0f, 1.0f, D3DCOLOR_ARGB(255, 0, 0, 255), }
	};
	const unsigned short Index[] =
	{
		0, 1,
		0, 2,
		0, 3
	};
	if (FAILED(pDirect3DDevice->CreateVertexBuffer(4 * sizeof(CUSTOMVERTEX), 0, D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &pBufferVertexAxes, NULL)))
		return E_FAIL;
	VOID* pBV;
	if(FAILED(pBufferVertexAxes->Lock(0, sizeof(Vertexes), (void**) &pBV, 0)))
		return E_FAIL;
	memcpy(pBV, Vertexes, sizeof(Vertexes));
	pBufferVertexAxes->Unlock();

	pDirect3DDevice->CreateIndexBuffer(4 * sizeof(Index), 0, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &pBufferIndex, NULL);
	VOID* pBI;
	pBufferIndex->Lock(0, sizeof(Index), (void**) &pBI, 0);
	memcpy(pBI, Index, sizeof(Index));
	pBufferIndex->Unlock();
	// Render axes
	pDirect3DDevice->SetStreamSource(0, pBufferVertexAxes, 0, sizeof(CUSTOMVERTEX));
	pDirect3DDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
	pDirect3DDevice->SetIndices(pBufferIndex);
	pDirect3DDevice->DrawIndexedPrimitive(D3DPT_LINELIST, 0, 0, 4, 0, 3);
	pBufferVertexAxes->Release();
	pBufferIndex->Release();
	return S_OK;
}


//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Draws the scene
//-----------------------------------------------------------------------------
VOID Render()
{
	if( NULL == pDirect3DDevice )
		return;

	// Clear the backbuffer to a blue color
	pDirect3DDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB( 10, 10, 10 ), 1.0f, 0 );

	// Begin the scene
	if( SUCCEEDED( pDirect3DDevice->BeginScene() ) )
	{
		
		// Rendering of scene objects can happen here
		// Axes
		D3DXMATRIX MatrixWorld;
		D3DXMatrixIdentity( &MatrixWorld );
		pDirect3DDevice->SetTransform(D3DTS_WORLD, &MatrixWorld);
		DrawAxes();
		
		// Translate particles
		Matrix();	
		ParticleRender();

		//End the scene
		pDirect3DDevice->EndScene();
		//TakeScreenShot();
	}

	// Present the backbuffer contents to the display
	pDirect3DDevice->Present( NULL, NULL, NULL, NULL );
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
		Cleanup();
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
		Render();
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

	// Initialize Direct3D
	if( SUCCEEDED( InitialDirect3D( hWnd ) ) )
	{

		// Show the window
		ShowWindow( hWnd, SW_SHOWDEFAULT );
		UpdateWindow( hWnd );
		// Initial
		string path;
		string pattern;
		Camera cam;
		int firstFrame, lastFrame, stepFrame;
		{
			ifstream fin("settings.txt");
			fin >> path;			
			fin >> pattern;			
			fin >> firstFrame >> lastFrame >> stepFrame;
			float x, y, z;
			fin >> x >> y >> z;
			D3DXVECTOR3 pos(x, y, z);
			cam.setPosition(&pos);
			fin >> x >> y >> z;
			D3DXVECTOR3 look(x, y, z);
			cam.setLook(&look);
			fin.close();				
		}
		InitialMatrix(cam);
		// Enter the message loop
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
			InputParticles(frame, path, pattern);
			Render();			
			TakeScreenShot(frame);			
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



