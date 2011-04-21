//--------------------------------------------------------------------------------------
// File: Render11.cpp
//
// This sample shows an simple implementation of the DirectX 11 Hardware Tessellator
// for rendering a Bezier Patch.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#include "DXUT.h"
#include "DXUTcamera.h"
#include "DXUTgui.h"
#include "DXUTsettingsDlg.h"
#include "SDKmisc.h"
#include "resource.h"

#include <time.h>
#include "Axis.h"
#include "gMetaballs.h"
#include "gParticlesRender.h"
#include "ParticlesContainer.h"
#include "Settings.h"

#define DEG2RAD( a ) ( a * D3DX_PI / 180.f )


//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
CDXUTDialogResourceManager          g_DialogResourceManager; // manager for shared resources of dialogs
CModelViewerCamera                  g_Camera;                // A model viewing camera
CD3DSettingsDlg                     g_D3DSettingsDlg;        // Device settings dialog
CDXUTDialog                         g_HUD;                   // manages the 3D   
CDXUTDialog                         g_SampleUI;              // dialog for sample specific controls

// Resources
CDXUTTextHelper*                    g_pTxtHelper = NULL;


D3DXMATRIX                          g_World;
float                               g_fScale = 0.0f;
float								g_fMetaballsSize = 0.0f;
bool                                g_bSpinning = false;
bool								g_Capture = false;

ParticlesContainer					particlesContainer;
gMetaballs							metaballs;
gParticlesRender					particleRender;
Settings							appSettings;
Axis								axis;

//--------------------------------------------------------------------------------------
// UI control IDs
//--------------------------------------------------------------------------------------
#define IDC_TOGGLEFULLSCREEN      1
#define IDC_TOGGLEREF             2
#define IDC_CHANGEDEVICE          3
#define IDC_TOGGLESPIN          4
#define IDC_VOLUME_SCALE          5
#define IDC_VOLUME_STATIC         6
#define IDC_TOGGLEWARP          7
#define IDC_META_STATIC			8
#define IDC_META_SCALE			9
#define IDC_RENDERTYPE			10

//--------------------------------------------------------------------------------------
// Forward declarations 
//--------------------------------------------------------------------------------------
bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, void* pUserContext );
void CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext );
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing,
	void* pUserContext );
void CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext );

bool CALLBACK IsD3D11DeviceAcceptable( const CD3D11EnumAdapterInfo *AdapterInfo, UINT Output, const CD3D11EnumDeviceInfo *DeviceInfo,
	DXGI_FORMAT BackBufferFormat, bool bWindowed, void* pUserContext );
HRESULT CALLBACK OnD3D11CreateDevice( ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc,
	void* pUserContext );
HRESULT CALLBACK OnD3D11ResizedSwapChain( ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain,
	const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext );
void CALLBACK OnD3D11ReleasingSwapChain( void* pUserContext );
void CALLBACK OnD3D11DestroyDevice( void* pUserContext );
void CALLBACK OnD3D11FrameRender( ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, double fTime,
	float fElapsedTime, void* pUserContext );
void CALLBACK KeyboardProc( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext );


void InitApp();
void RenderText();

//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	// DXUT will create and use the best device (either D3D10 or D3D11) 
	// that is available on the system depending on which D3D callbacks are set below

	// Set DXUT callbacks
	DXUTSetCallbackDeviceChanging( ModifyDeviceSettings );
	DXUTSetCallbackMsgProc( MsgProc );
	DXUTSetCallbackKeyboard( KeyboardProc );
	DXUTSetCallbackFrameMove( OnFrameMove );

	DXUTSetCallbackD3D11DeviceAcceptable( IsD3D11DeviceAcceptable );
	DXUTSetCallbackD3D11DeviceCreated( OnD3D11CreateDevice );
	DXUTSetCallbackD3D11SwapChainResized( OnD3D11ResizedSwapChain );
	DXUTSetCallbackD3D11FrameRender( OnD3D11FrameRender );
	DXUTSetCallbackD3D11SwapChainReleasing( OnD3D11ReleasingSwapChain );
	DXUTSetCallbackD3D11DeviceDestroyed( OnD3D11DestroyDevice );

	InitApp();
	DXUTInit( true, true ); // Parse the command line, show msgboxes on error, and an extra cmd line param to force REF for now
	DXUTSetCursorSettings( true, true ); // Show the cursor and clip it when in full screen
	DXUTCreateWindow( L"Render11" );
	DXUTCreateDevice( D3D_FEATURE_LEVEL_11_0,  true, appSettings.screenWidth, appSettings.screenHeight );
	DXUTMainLoop(); // Enter into the DXUT render loop

	return DXUTGetExitCode();
}

void DisableMetaballsGUI()
{
	g_SampleUI.GetStatic(IDC_META_STATIC)->SetVisible(FALSE);
	g_SampleUI.GetSlider(IDC_META_SCALE)->SetVisible(FALSE);
}

void EnableMetaballsGUI()
{
	g_SampleUI.GetStatic(IDC_META_STATIC)->SetVisible(TRUE);
	g_SampleUI.GetSlider(IDC_META_SCALE)->SetVisible(TRUE);
}

void InitGUI()
{
	g_D3DSettingsDlg.Init( &g_DialogResourceManager );
	g_HUD.Init( &g_DialogResourceManager );
	g_SampleUI.Init( &g_DialogResourceManager );

	g_HUD.SetCallback( OnGUIEvent ); int iY = 10;
	g_HUD.AddButton( IDC_TOGGLEFULLSCREEN, L"Toggle full screen", 35, iY, 125, 22 );
	g_HUD.AddButton( IDC_CHANGEDEVICE, L"Change device (F2)", 35, iY += 24, 125, 22, VK_F2 );
	g_HUD.AddButton( IDC_TOGGLEREF, L"Toggle REF (F3)", 35, iY += 24, 125, 22, VK_F3 );
	g_HUD.AddButton( IDC_TOGGLEWARP, L"Toggle WARP (F4)", 35, iY += 24, 125, 22, VK_F4 );

	D3DXCOLOR bg( 0.0f, 0.125f, 0.3f, 0.2f);
	//g_SampleUI.SetBackgroundColors(bg, bg, bg, bg);
	g_SampleUI.SetCallback( OnGUIEvent ); iY = 10;
	g_SampleUI.AddComboBox( IDC_RENDERTYPE, 35, iY += 24, 120, 22, 0, false);
	g_SampleUI.GetComboBox( IDC_RENDERTYPE)->SetDropHeight(40);
	g_SampleUI.GetComboBox( IDC_RENDERTYPE)->AddItem( L"Metaballs", (void*) 0);
	g_SampleUI.GetComboBox( IDC_RENDERTYPE)->AddItem( L"Particles", (void*) 1);	


	WCHAR sz[100];
	iY += 24;
	swprintf_s( sz, 100, L"Volume scale: %0.2f", g_fScale );
	g_SampleUI.AddStatic( IDC_VOLUME_STATIC, sz, 35, iY += 24, 125, 22 );			
	g_SampleUI.AddSlider( IDC_VOLUME_SCALE, 50, iY += 24, 100, 22, 0, appSettings.volumeResolution * 100, ( int )( g_fScale * 100.0f ) );

	iY += 24;
	swprintf_s( sz, 100, L"Metaballs size: %0.2f", g_fMetaballsSize );
	g_SampleUI.AddStatic( IDC_META_STATIC, sz, 35, iY += 24, 125, 22 );
	g_SampleUI.AddSlider( IDC_META_SCALE, 50, iY += 24, 100, 22, 1, 128 * 100, ( int )( g_fMetaballsSize * 100.0f ) );

	switch (appSettings.renderState)
	{
	case PARTICLES:
		{		
			DisableMetaballsGUI();
			g_SampleUI.GetComboBox( IDC_RENDERTYPE)->SetSelectedByIndex(1);
			break;
		}
	case METABALLS:
		{
			g_SampleUI.GetComboBox( IDC_RENDERTYPE)->SetSelectedByIndex(0);
			break;
		}

	}
}

//--------------------------------------------------------------------------------------
// Initialize the app 
//--------------------------------------------------------------------------------------
void InitApp()
{
	g_fScale = 140.0f;
	g_fMetaballsSize = 20.0f;
	g_bSpinning = false;

	appSettings.loadFromFile("settings.txt");
	particlesContainer.init(appSettings.pathToFolder, appSettings.patternString, appSettings.firstFrame, appSettings.lastFrame, appSettings.stepFrame);	

	InitGUI();
}


//--------------------------------------------------------------------------------------
// Called right before creating a D3D9 or D3D10 device, allowing the app to modify the device settings as needed
//--------------------------------------------------------------------------------------
bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, void* pUserContext )
{
	// For the first device created if its a REF device, optionally display a warning dialog box
	static bool s_bFirstTime = true;
	if( s_bFirstTime )
	{
		s_bFirstTime = false;
		if( ( DXUT_D3D9_DEVICE == pDeviceSettings->ver && pDeviceSettings->d3d9.DeviceType == D3DDEVTYPE_REF ) ||
			( DXUT_D3D11_DEVICE == pDeviceSettings->ver &&
			pDeviceSettings->d3d11.DriverType == D3D_DRIVER_TYPE_REFERENCE ) )
		{
			DXUTDisplaySwitchingToREFWarning( pDeviceSettings->ver );
		}
	}

	return true;
}


//--------------------------------------------------------------------------------------
// Handle updates to the scene
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext )
{
	// Update the camera's position based on user input 
	g_Camera.FrameMove( fElapsedTime );
	if( g_bSpinning )
		D3DXMatrixRotationY( &g_World, 60.0f * DEG2RAD((float)fTime) );
	else
		D3DXMatrixRotationY( &g_World, DEG2RAD( 180.0f ) );
	D3DXMATRIX mTranslate;
	D3DXMatrixTranslation(&mTranslate, -0.5f, -0.5f, -0.5f);
	D3DXMATRIX mRot;
	D3DXMatrixRotationX( &mRot, DEG2RAD( -90.0f ) );
	g_World = mTranslate * mRot * g_World;	
	D3DXMATRIX view =  g_World * (*g_Camera.GetViewMatrix());
	axis.onFrameMove(g_World * (*g_Camera.GetViewMatrix()) * (*g_Camera.GetProjMatrix()));
	switch(appSettings.renderState)
	{
	case METABALLS:
		{
			metaballs.onFrameMove(g_World * (*g_Camera.GetViewMatrix()) * (*g_Camera.GetProjMatrix()));
			break;
		}
	case PARTICLES:
		{
			particleRender.onFrameMove(g_World * (*g_Camera.GetViewMatrix()) * (*g_Camera.GetProjMatrix()), view);
			break;
		}
	}
}


//--------------------------------------------------------------------------------------
// Render the help and statistics text
//--------------------------------------------------------------------------------------
void RenderText()
{
	g_pTxtHelper->Begin();
	g_pTxtHelper->SetInsertionPos( 2, 0 );
	g_pTxtHelper->SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 0.0f, 1.0f ) );
	g_pTxtHelper->DrawTextLine( DXUTGetFrameStats( DXUTIsVsyncEnabled() ) );
	g_pTxtHelper->DrawTextLine( DXUTGetDeviceStats() );

	g_pTxtHelper->End();
}


//--------------------------------------------------------------------------------------
// Handle messages to the application
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing,
	void* pUserContext )
{
	// Pass messages to dialog resource manager calls so GUI state is updated correctly
	*pbNoFurtherProcessing = g_DialogResourceManager.MsgProc( hWnd, uMsg, wParam, lParam );
	if( *pbNoFurtherProcessing )
		return 0;

	// Pass messages to settings dialog if its active
	if( g_D3DSettingsDlg.IsActive() )
	{
		g_D3DSettingsDlg.MsgProc( hWnd, uMsg, wParam, lParam );
		return 0;
	}

	// Give the dialogs a chance to handle the message first
	*pbNoFurtherProcessing = g_HUD.MsgProc( hWnd, uMsg, wParam, lParam );
	if( *pbNoFurtherProcessing )
		return 0;
	*pbNoFurtherProcessing = g_SampleUI.MsgProc( hWnd, uMsg, wParam, lParam );
	if( *pbNoFurtherProcessing )
		return 0;

	// Pass all remaining windows messages to camera so it can respond to user input
	g_Camera.HandleMessages( hWnd, uMsg, wParam, lParam );

	return 0;
}

//--------------------------------------------------------------------------------------
// Handles the GUI events
//--------------------------------------------------------------------------------------
void CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext )
{
	switch( nControlID )
	{
		// Standard DXUT controls
	case IDC_TOGGLEFULLSCREEN:
		DXUTToggleFullScreen(); break;
	case IDC_TOGGLEREF:
		DXUTToggleREF(); break;
	case IDC_CHANGEDEVICE:
		g_D3DSettingsDlg.SetActive( !g_D3DSettingsDlg.IsActive() ); break;
	case IDC_TOGGLESPIN:
		{
			g_bSpinning = g_SampleUI.GetCheckBox( IDC_TOGGLESPIN )->GetChecked();
			break;
		}

	case IDC_VOLUME_SCALE:
		{
			if (nEvent == EVENT_SLIDER_VALUE_CHANGED)
			{
				WCHAR sz[100];
				g_fScale = ( float )( g_SampleUI.GetSlider( IDC_VOLUME_SCALE )->GetValue() * 0.01f );
				swprintf_s( sz, 100, L"Volume scale: %0.2f", g_fScale );
				g_SampleUI.GetStatic( IDC_VOLUME_STATIC )->SetText( sz );
				if (appSettings.renderState == METABALLS)
				{
					metaballs.updateVolume(particlesContainer.getParticles(), particlesContainer.getParticlesCount(), g_fScale, g_fMetaballsSize);
				}
				else if (appSettings.renderState == PARTICLES)
				{
					particleRender.updateParticles(particlesContainer.getParticles(), g_fScale);
				}

				break;
			}
		}
	case IDC_META_SCALE:
		{
			if (nEvent == EVENT_SLIDER_VALUE_CHANGED)
			{
				WCHAR sz[100];
				g_fMetaballsSize = ( float )( g_SampleUI.GetSlider( IDC_META_SCALE )->GetValue() * 0.01f );
				swprintf_s( sz, 100, L"Metaballs scale: %0.2f", g_fMetaballsSize );
				g_SampleUI.GetStatic( IDC_META_STATIC )->SetText( sz );
				metaballs.updateVolume(particlesContainer.getParticles(), particlesContainer.getParticlesCount(), g_fScale, g_fMetaballsSize);
				break;
			}
		}
	case IDC_RENDERTYPE:
		{
			if (nEvent == EVENT_COMBOBOX_SELECTION_CHANGED)
			{
				int renderType = (int) g_SampleUI.GetComboBox(IDC_RENDERTYPE)->GetSelectedIndex();
				if (renderType == 0)
				{
					appSettings.renderState = METABALLS;
					EnableMetaballsGUI();
					metaballs.updateVolume(particlesContainer.getParticles(), particlesContainer.getParticlesCount(), g_fScale, g_fMetaballsSize);
				}
				else if (renderType == 1)
				{
					appSettings.renderState = PARTICLES;					
					DisableMetaballsGUI();			
					particleRender.updateParticles(particlesContainer.getParticles(), g_fScale);
				}
			}			
		}
	}
}

//--------------------------------------------------------------------------------------
// Reject any D3D11 devices that aren't acceptable by returning false
//--------------------------------------------------------------------------------------
bool CALLBACK IsD3D11DeviceAcceptable( const CD3D11EnumAdapterInfo *AdapterInfo, UINT Output, const CD3D11EnumDeviceInfo *DeviceInfo,
	DXGI_FORMAT BackBufferFormat, bool bWindowed, void* pUserContext )
{
	return true;
}

////--------------------------------------------------------------------------------------
//// Find and compile the specified shader
////--------------------------------------------------------------------------------------
//HRESULT CompileShaderFromFile( WCHAR* szFileName, D3D_SHADER_MACRO* pDefines, LPCSTR szEntryPoint, 
//	LPCSTR szShaderModel, ID3DBlob** ppBlobOut )
//{
//	HRESULT hr = S_OK;
//
//	// find the file
//	WCHAR str[MAX_PATH];
//	V_RETURN( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, szFileName ) );
//
//	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
//#if defined( DEBUG ) || defined( _DEBUG )
//	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
//	// Setting this flag improves the shader debugging experience, but still allows 
//	// the shaders to be optimized and to run exactly the way they will run in 
//	// the release configuration of this program.
//	dwShaderFlags |= D3DCOMPILE_DEBUG;
//#endif
//
//	ID3DBlob* pErrorBlob;
//	hr = D3DX11CompileFromFile( str, pDefines, NULL, szEntryPoint, szShaderModel, 
//		dwShaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL );
//	if( FAILED(hr) )
//	{
//		if( pErrorBlob != NULL )
//			OutputDebugStringA( (char*)pErrorBlob->GetBufferPointer() );
//		SAFE_RELEASE( pErrorBlob );
//		return hr;
//	}
//	SAFE_RELEASE( pErrorBlob );
//
//	return S_OK;
//}

//--------------------------------------------------------------------------------------
// Create any D3D11 resources that aren't dependant on the back buffer
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D11CreateDevice( ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc,
	void* pUserContext )
{
	HRESULT hr;

	ID3D11DeviceContext* pd3dImmediateContext = DXUTGetD3D11DeviceContext();
	V_RETURN( g_DialogResourceManager.OnD3D11CreateDevice( pd3dDevice, pd3dImmediateContext ) );
	V_RETURN( g_D3DSettingsDlg.OnD3D11CreateDevice( pd3dDevice ) );
	g_pTxtHelper = new CDXUTTextHelper( pd3dDevice, pd3dImmediateContext, &g_DialogResourceManager, 15 );

	// Initialize the world matrices
	D3DXMatrixIdentity( &g_World );

	// Initialize the camera
	g_Camera.SetViewParams( &appSettings.cameraPos, &appSettings.cameraLookAt);

	metaballs.init(pd3dDevice, pd3dImmediateContext, appSettings.screenWidth, appSettings.screenHeight, appSettings.volumeResolution);
	metaballs.updateVolume(particlesContainer.getParticles(), particlesContainer.getParticlesCount(), g_fScale, g_fMetaballsSize);
	particleRender.init(pd3dDevice, pd3dImmediateContext);
	particleRender.updateParticles(particlesContainer.getParticles(), g_fScale);

	axis.init(pd3dDevice, pd3dImmediateContext);

	return S_OK;
}


//--------------------------------------------------------------------------------------
// Create any D3D11 resources that depend on the back buffer
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D11ResizedSwapChain( ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain,
	const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
	HRESULT hr;

	V_RETURN( g_DialogResourceManager.OnD3D11ResizedSwapChain( pd3dDevice, pBackBufferSurfaceDesc ) );
	V_RETURN( g_D3DSettingsDlg.OnD3D11ResizedSwapChain( pd3dDevice, pBackBufferSurfaceDesc ) );

	// Setup the camera's projection parameters
	float fAspectRatio = pBackBufferSurfaceDesc->Width / ( FLOAT )pBackBufferSurfaceDesc->Height;
	g_Camera.SetProjParams( D3DX_PI / 4, fAspectRatio, 0.1f, 20.0f );
	g_Camera.SetWindow( pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height );
	g_Camera.SetButtonMasks( MOUSE_MIDDLE_BUTTON, MOUSE_WHEEL, MOUSE_LEFT_BUTTON );

	g_HUD.SetLocation( pBackBufferSurfaceDesc->Width - 170, 0 );
	g_HUD.SetSize( 170, 170 );
	g_SampleUI.SetLocation( pBackBufferSurfaceDesc->Width - 170, pBackBufferSurfaceDesc->Height - 300 );
	g_SampleUI.SetSize( 170, 300 );
	switch(appSettings.renderState)
	{
	case METABALLS:
		{
			metaballs.onFrameResize(pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height);
			break;
		}
	case PARTICLES:
		{			
			break;
		}
	}
	return S_OK;
}

void CaptureScreen(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pDevContext, char* fileName)
{
	HRESULT hr;
	
	ID3D11RenderTargetView* pRTV = DXUTGetD3D11RenderTargetView();	
	ID3D11Resource *backbufferRes;
	pRTV->GetResource(&backbufferRes);
	D3D11_RENDER_TARGET_VIEW_DESC rtDesc;
	pRTV->GetDesc(&rtDesc);

	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.ArraySize = 1;
	texDesc.BindFlags = 0;
	texDesc.CPUAccessFlags = 0;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.Width = appSettings.screenWidth;  // must be same as backbuffer
	texDesc.Height = appSettings.screenHeight; // must be same as backbuffer
	texDesc.MipLevels = 1;
	texDesc.MiscFlags = 0;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;

	ID3D11Texture2D *texture;
	HR( pd3dDevice->CreateTexture2D(&texDesc, 0, &texture) );
	pDevContext->CopyResource(texture, backbufferRes);

	V( D3DX11SaveTextureToFileA(pDevContext, texture, D3DX11_IFF_PNG, fileName) );
	texture->Release();
	backbufferRes->Release();
}

//--------------------------------------------------------------------------------------
// Render the scene using the D3D11 device
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D11FrameRender( ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, double fTime,
	float fElapsedTime, void* pUserContext )
{
	if (g_Capture)
	{
		char fileName[64] = {};
		char date[32] = {};
		char time[32] = {};
		_strdate_s(date, sizeof(date));
		_strtime_s(time, sizeof(time));
		sprintf_s(fileName, "frame%d-(%c%c_%c%c_%c%c-%c%c_%c%c_%c%c).png", particlesContainer.getNumCurrFrame(), date[0], date[1], date[3], date[4], date[6], date[7], time[0], time[1], time[3], time[4], time[6], time[7]);
		CaptureScreen(pd3dDevice, pd3dImmediateContext, fileName);
		g_Capture = FALSE;
	}

	// If the settings dialog is being shown, then render it instead of rendering the app's scene
	if( g_D3DSettingsDlg.IsActive() )
	{
		g_D3DSettingsDlg.OnRender( fElapsedTime );
		return;
	}

	// Clear the render target and depth stencil
	float ClearColor[4] = { 0.552f, 0.713f, 0.803f, 1.0f };
	ID3D11RenderTargetView* pRTV = DXUTGetD3D11RenderTargetView();
	pd3dImmediateContext->ClearRenderTargetView( pRTV, ClearColor );
	ID3D11DepthStencilView* pDSV = DXUTGetD3D11DepthStencilView();
	pd3dImmediateContext->ClearDepthStencilView( pDSV, D3D11_CLEAR_DEPTH, 1.0, 0 );

	axis.draw();

	switch(appSettings.renderState)
	{
	case METABALLS:
		{
			metaballs.draw();
			break;
		}
	case PARTICLES:
		{
			particleRender.draw();
			break;
		}
	}



	//
	// Render the UI
	//
	g_HUD.OnRender( fElapsedTime );
	g_SampleUI.OnRender( fElapsedTime );

	// For debug only
	//if(particlesContainer.getNumCurrFrame() + 1 < appSettings.lastFrame)
	//{
	//	particlesContainer.getNextFrame();
	//	switch(appSettings.renderState)
	//	{
	//	case METABALLS:
	//		{
	//			metaballs.updateVolume(particlesContainer.getParticles(), particlesContainer.getParticlesCount(), g_fScale, g_fMetaballsSize);						
	//			break;
	//		}
	//	case PARTICLES:
	//		{
	//			particleRender.updateParticles(particlesContainer.getParticles(), g_fScale);
	//			break;
	//		}
	//	}	
	//}
	//else
	//{
	//	exit(0);
	//}

	// Render the HUD
	DXUT_BeginPerfEvent( DXUT_PERFEVENTCOLOR, L"HUD / Stats" );
	g_HUD.OnRender( fElapsedTime );
	g_SampleUI.OnRender( fElapsedTime );
	RenderText();
	DXUT_EndPerfEvent();
}


//--------------------------------------------------------------------------------------
// Release D3D11 resources created in OnD3D10ResizedSwapChain 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D11ReleasingSwapChain( void* pUserContext )
{
	g_DialogResourceManager.OnD3D11ReleasingSwapChain();
}


//--------------------------------------------------------------------------------------
// Release D3D11 resources created in OnD3D10CreateDevice 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D11DestroyDevice( void* pUserContext )
{
	g_DialogResourceManager.OnD3D11DestroyDevice();
	g_D3DSettingsDlg.OnD3D11DestroyDevice();
	DXUTGetGlobalResourceCache().OnDestroyDevice();
	SAFE_DELETE( g_pTxtHelper );
}

//--------------------------------------------------------------------------------------
// Handle key presses
//--------------------------------------------------------------------------------------
void CALLBACK KeyboardProc( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext )
{
	if( bKeyDown )
	{
		switch( nChar )
		{
		case VK_F1:
			break;
		case 'r':
		case 'R':
			{
				particlesContainer.getFrame(appSettings.firstFrame);
				switch(appSettings.renderState)
				{
				case METABALLS:
					{
						metaballs.updateVolume(particlesContainer.getParticles(), particlesContainer.getParticlesCount(), g_fScale, g_fMetaballsSize);
						break;
					}
				case PARTICLES:
					{
						particleRender.updateParticles(particlesContainer.getParticles(), g_fScale);
						break;
					}
				}
				break;
			}
		case 'n':
		case 'N':
			{
				particlesContainer.getNextFrame();
				switch(appSettings.renderState)
				{
				case METABALLS:
					{
						metaballs.updateVolume(particlesContainer.getParticles(), particlesContainer.getParticlesCount(), g_fScale, g_fMetaballsSize);						
						break;
					}
				case PARTICLES:
					{
						particleRender.updateParticles(particlesContainer.getParticles(), g_fScale);
						break;
					}
				}				
				break;
			}
		case 'b':
		case 'B':
			{
				particlesContainer.getPrevFrame();
				switch(appSettings.renderState)
				{
				case METABALLS:
					{
						metaballs.updateVolume(particlesContainer.getParticles(), particlesContainer.getParticlesCount(), g_fScale, g_fMetaballsSize);						
						break;
					}
				case PARTICLES:
					{
						particleRender.updateParticles(particlesContainer.getParticles(), g_fScale);
						break;
					}
				}				
				break;
			}
		case VK_F9:
			{
				g_Capture = true;
				break;
			}
		}
	}
}



