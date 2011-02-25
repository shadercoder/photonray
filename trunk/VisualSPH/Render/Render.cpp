//--------------------------------------------------------------------------------------
// File: Render.cpp
//
//--------------------------------------------------------------------------------------
#include "DXUT.h"
#include "DXUTmisc.h"
#include "DXUTcamera.h"
#include "DXUTgui.h"
#include "DXUTsettingsDlg.h"
#include "SDKmisc.h"
#include "SDKmesh.h"
#include "resource.h"

#include "Axis.h"

#include "gMetaballs.h"
#include "gParticlesRender.h"
#include "ParticlesContainer.h"
#include "Settings.h"

#define DEG2RAD( a ) ( a * D3DX_PI / 180.f )

//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------
CModelViewerCamera                  g_Camera;               // A model viewing camera
CDXUTDialogResourceManager          g_DialogResourceManager;// manager for shared resources of dialogs
CD3DSettingsDlg                     g_D3DSettingsDlg;       // Device settings dialog
CDXUTDialog                         g_HUD;                  // manages the 3D UI
CDXUTDialog                         g_SampleUI;             // dialog for sample specific controls

ID3DX10Font*                        g_pFont = NULL;         // Font for drawing text
ID3DX10Sprite*                      g_pSprite = NULL;       // Sprite for batching text drawing
CDXUTTextHelper*                    g_pTxtHelper = NULL;

D3DXMATRIX                          g_World;
float                               g_fScale = 0.0f;
float								g_fMetaballsSize = 0.0f;
bool                                g_bSpinning = false;

ParticlesContainer					particlesContainer;
gMetaballs							metaballs;
gParticlesRender					particleRender;
Settings							appSettings;

//--------------------------------------------------------------------------------------
// UI control IDs
//--------------------------------------------------------------------------------------
#define IDC_TOGGLEFULLSCREEN    1
#define IDC_TOGGLEREF           2
#define IDC_CHANGEDEVICE        3
#define IDC_TOGGLESPIN          4
#define IDC_PUFF_SCALE          5
#define IDC_PUFF_STATIC         6
#define IDC_TOGGLEWARP          7
#define IDC_META_STATIC			8
#define IDC_META_SCALE			9

//--------------------------------------------------------------------------------------
// Forward declarations 
//--------------------------------------------------------------------------------------
bool CALLBACK IsD3D10DeviceAcceptable( UINT Adapter, UINT Output, D3D10_DRIVER_TYPE DeviceType,
	DXGI_FORMAT BufferFormat, bool bWindowed, void* pUserContext );
HRESULT CALLBACK OnD3D10CreateDevice( ID3D10Device* pd3dDevice, const DXGI_SURFACE_DESC* pBufferSurfaceDesc,
	void* pUserContext );
HRESULT CALLBACK OnD3D10ResizedSwapChain( ID3D10Device* pd3dDevice, IDXGISwapChain* pSwapChain,
	const DXGI_SURFACE_DESC* pBufferSurfaceDesc, void* pUserContext );
void CALLBACK OnD3D10FrameRender( ID3D10Device* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext );
void CALLBACK OnD3D10ReleasingSwapChain( void* pUserContext );
void CALLBACK OnD3D10DestroyDevice( void* pUserContext );
void CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext );

LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing,
	void* pUserContext );
void CALLBACK KeyboardProc( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext );
void CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext );
bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, void* pUserContext );

void RenderText();
void InitApp();


//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
INT WINAPI wWinMain( HINSTANCE, HINSTANCE, LPWSTR, int )
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	// DXUT will create and use the best device (either D3D9 or D3D10) 
	// that is available on the system depending on which D3D callbacks are set below

	// Set DXUT callbacks
	DXUTSetCallbackD3D10DeviceAcceptable( IsD3D10DeviceAcceptable );
	DXUTSetCallbackD3D10DeviceCreated( OnD3D10CreateDevice );
	DXUTSetCallbackD3D10SwapChainResized( OnD3D10ResizedSwapChain );
	DXUTSetCallbackD3D10SwapChainReleasing( OnD3D10ReleasingSwapChain );
	DXUTSetCallbackD3D10DeviceDestroyed( OnD3D10DestroyDevice );
	DXUTSetCallbackD3D10FrameRender( OnD3D10FrameRender );

	DXUTSetCallbackMsgProc( MsgProc );
	DXUTSetCallbackKeyboard( KeyboardProc );
	DXUTSetCallbackFrameMove( OnFrameMove );
	DXUTSetCallbackDeviceChanging( ModifyDeviceSettings );

	DXUTInit( true, true, NULL ); // Parse the command line, show msgboxes on error, no extra command line params
	DXUTSetCursorSettings( true, true ); // Show the cursor and clip it when in full screen

	InitApp();

	DXUTCreateWindow( L"VisualSPH" );
	DXUTCreateDevice( true, appSettings.screenWidth, appSettings.screenHeight);
	DXUTMainLoop(); // Enter into the DXUT render loop

	return DXUTGetExitCode();
}


//--------------------------------------------------------------------------------------
// Initialize the app 
//--------------------------------------------------------------------------------------
void InitApp()
{
	g_fScale = 1.0f;
	g_fMetaballsSize = 1.0f;
	g_bSpinning = false;

	appSettings.loadFromFile("settings.txt");
	particlesContainer.init(appSettings.pathToFolder, appSettings.patternString, appSettings.firstFrame, appSettings.lastFrame, appSettings.stepFrame);	

	g_D3DSettingsDlg.Init( &g_DialogResourceManager );
	g_HUD.Init( &g_DialogResourceManager );
	g_SampleUI.Init( &g_DialogResourceManager );

	g_HUD.SetCallback( OnGUIEvent ); int iY = 10;
	g_HUD.AddButton( IDC_TOGGLEFULLSCREEN, L"Toggle full screen", 35, iY, 125, 22 );
	g_HUD.AddButton( IDC_CHANGEDEVICE, L"Change device (F2)", 35, iY += 24, 125, 22, VK_F2 );
	g_HUD.AddButton( IDC_TOGGLEREF, L"Toggle REF (F3)", 35, iY += 24, 125, 22, VK_F3 );
	g_HUD.AddButton( IDC_TOGGLEWARP, L"Toggle WARP (F4)", 35, iY += 24, 125, 22, VK_F4 );

	g_SampleUI.SetCallback( OnGUIEvent ); iY = 10;

	switch (appSettings.renderState)
	{
	case METABALLS:
		{
			WCHAR sz[100];
			iY += 24;
			swprintf_s( sz, 100, L"Volume scale: %0.2f", g_fScale );
			g_SampleUI.AddStatic( IDC_PUFF_STATIC, sz, 35, iY += 24, 125, 22 );
			g_SampleUI.AddSlider( IDC_PUFF_SCALE, 50, iY += 24, 100, 22, 0, appSettings.volumeResolution * 100, ( int )( g_fScale * 100.0f ) );

			iY += 24;
			swprintf_s( sz, 100, L"Metaballs size: %0.2f", g_fMetaballsSize );
			g_SampleUI.AddStatic( IDC_META_STATIC, sz, 35, iY += 24, 125, 22 );
			g_SampleUI.AddSlider( IDC_META_SCALE, 50, iY += 24, 100, 22, 1, 16 * 100, ( int )( g_fMetaballsSize * 100.0f ) );
			break;
		}
	}

	iY += 24;
	g_SampleUI.AddCheckBox( IDC_TOGGLESPIN, L"Toggle Spinning", 35, iY += 24, 125, 22, g_bSpinning );

}


//--------------------------------------------------------------------------------------
// Reject any D3D10 devices that aren't acceptable by returning false
//--------------------------------------------------------------------------------------
bool CALLBACK IsD3D10DeviceAcceptable( UINT Adapter, UINT Output, D3D10_DRIVER_TYPE DeviceType,
	DXGI_FORMAT BufferFormat, bool bWindowed, void* pUserContext )
{
	return true;
}


//--------------------------------------------------------------------------------------
// Create any D3D10 resources that aren't dependant on the back buffer
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D10CreateDevice( ID3D10Device* pd3dDevice, const DXGI_SURFACE_DESC* pBufferSurfaceDesc,
	void* pUserContext )
{
	HRESULT hr;

	V_RETURN( g_DialogResourceManager.OnD3D10CreateDevice( pd3dDevice ) );
	V_RETURN( g_D3DSettingsDlg.OnD3D10CreateDevice( pd3dDevice ) );
	V_RETURN( D3DX10CreateFont( pd3dDevice, 15, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
		L"Arial", &g_pFont ) );
	V_RETURN( D3DX10CreateSprite( pd3dDevice, 512, &g_pSprite ) );
	g_pTxtHelper = new CDXUTTextHelper( NULL, NULL, g_pFont, g_pSprite, 15 );

	// Initialize the world matrices
	D3DXMatrixIdentity( &g_World );

	// Initialize the camera
	D3DXVECTOR3 Eye( 2.0f, 2.0f, 0.0f );
	D3DXVECTOR3 At( 0.0f, 0.0f, 0.0f );
	g_Camera.SetViewParams( &Eye, &At );

	metaballs.init(pd3dDevice, appSettings.screenWidth, appSettings.screenHeight, appSettings.volumeResolution);
	metaballs.updateVolume(particlesContainer.getParticles(), particlesContainer.getParticlesCount(), g_fScale, g_fMetaballsSize);
	particleRender.init(pd3dDevice);
	particleRender.updateParticles(particlesContainer.getParticles());
	return S_OK;
}


//--------------------------------------------------------------------------------------
// Create any D3D10 resources that depend on the back buffer
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D10ResizedSwapChain( ID3D10Device* pd3dDevice, IDXGISwapChain* pSwapChain,
	const DXGI_SURFACE_DESC* pBufferSurfaceDesc, void* pUserContext )
{
	HRESULT hr;

	V_RETURN( g_DialogResourceManager.OnD3D10ResizedSwapChain( pd3dDevice, pBufferSurfaceDesc ) );
	V_RETURN( g_D3DSettingsDlg.OnD3D10ResizedSwapChain( pd3dDevice, pBufferSurfaceDesc ) );

	// Setup the camera's projection parameters
	float fAspectRatio = static_cast<float>( pBufferSurfaceDesc->Width ) /
		static_cast<float>( pBufferSurfaceDesc->Height );
	g_Camera.SetProjParams( D3DX_PI / 4, fAspectRatio, 0.1f, 5000.0f );
	g_Camera.SetWindow( pBufferSurfaceDesc->Width, pBufferSurfaceDesc->Height );
	g_Camera.SetButtonMasks( MOUSE_MIDDLE_BUTTON, MOUSE_WHEEL, MOUSE_LEFT_BUTTON );

	g_HUD.SetLocation( pBufferSurfaceDesc->Width - 170, 0 );
	g_HUD.SetSize( 170, 170 );
	g_SampleUI.SetLocation( pBufferSurfaceDesc->Width - 170, pBufferSurfaceDesc->Height - 300 );
	g_SampleUI.SetSize( 170, 300 );
	switch(appSettings.renderState)
	{
	case METABALLS:
		{
			metaballs.onFrameResize(pBufferSurfaceDesc->Width, pBufferSurfaceDesc->Height);
			break;
		}
	case PARTICLES:
		{			
			break;
		}
	}

	return S_OK;
}


//--------------------------------------------------------------------------------------
// Render the scene using the D3D10 device
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D10FrameRender( ID3D10Device* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext )
{
	//
	// Clear the back buffer
	//
	float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; // red, green, blue, alpha
	ID3D10RenderTargetView* pRTV = DXUTGetD3D10RenderTargetView();
	pd3dDevice->ClearRenderTargetView( pRTV, ClearColor );

	// If the settings dialog is being shown, then
	// render it instead of rendering the app's scene
	if( g_D3DSettingsDlg.IsActive() )
	{
		g_D3DSettingsDlg.OnRender( fElapsedTime );
		return;
	}

	//
	// Clear the depth stencil
	//
	ID3D10DepthStencilView* pDSV = DXUTGetD3D10DepthStencilView();
	pd3dDevice->ClearDepthStencilView( pDSV, D3D10_CLEAR_DEPTH, 1.0, 0 );

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

	RenderText();
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
	wchar_t text[32] = {};
	swprintf_s(text, 32, L"Frame: %d",particlesContainer.getNumCurrFrame());
	g_pTxtHelper->DrawTextLine(text);
	swprintf_s(text, 32, L"Particles: %d",particlesContainer.getParticlesCount());
	g_pTxtHelper->DrawTextLine(text);
	g_pTxtHelper->End();
}


//--------------------------------------------------------------------------------------
// Release D3D10 resources created in OnD3D10ResizedSwapChain 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D10ReleasingSwapChain( void* pUserContext )
{
	g_DialogResourceManager.OnD3D10ReleasingSwapChain();
}


//--------------------------------------------------------------------------------------
// Release D3D10 resources created in OnD3D10CreateDevice 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D10DestroyDevice( void* pUserContext )
{
	g_DialogResourceManager.OnD3D10DestroyDevice();
	g_D3DSettingsDlg.OnD3D10DestroyDevice();
	DXUTGetGlobalResourceCache().OnDestroyDevice();
	SAFE_RELEASE( g_pFont );
	SAFE_RELEASE( g_pSprite );
	SAFE_DELETE( g_pTxtHelper );
}


//--------------------------------------------------------------------------------------
// Called right before creating a D3D9 or D3D10 device, allowing the app to modify the device settings as needed
//--------------------------------------------------------------------------------------
bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, void* pUserContext )
{
	return true;
}


//--------------------------------------------------------------------------------------
// This callback function will be called once at the beginning of every frame. This is the
// best location for your application to handle updates to the scene, but is not 
// intended to contain actual rendering calls, which should instead be placed in the 
// OnFrameRender callback.  
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
// Before handling window messages, DXUT passes incoming windows 
// messages to the application through this callback function. If the application sets 
// *pbNoFurtherProcessing to TRUE, then DXUT will not process this message.
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
						particleRender.updateParticles(particlesContainer.getParticles());
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
						particleRender.updateParticles(particlesContainer.getParticles());
						break;
					}
				}				
				break;
			}

		}
	}
}


//--------------------------------------------------------------------------------------
// Handles the GUI events
//--------------------------------------------------------------------------------------
void CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext )
{
	switch( nControlID )
	{
	case IDC_TOGGLEFULLSCREEN:
		DXUTToggleFullScreen(); break;
	case IDC_TOGGLEREF:
		DXUTToggleREF(); break;
	case IDC_CHANGEDEVICE:
		g_D3DSettingsDlg.SetActive( !g_D3DSettingsDlg.IsActive() ); break;
	case IDC_TOGGLEWARP:
		DXUTToggleWARP(); break;
	case IDC_TOGGLESPIN:
		{
			g_bSpinning = g_SampleUI.GetCheckBox( IDC_TOGGLESPIN )->GetChecked();
			break;
		}

	case IDC_PUFF_SCALE:
		{
			WCHAR sz[100];
			g_fScale = ( float )( g_SampleUI.GetSlider( IDC_PUFF_SCALE )->GetValue() * 0.01f );
			swprintf_s( sz, 100, L"Volume scale: %0.2f", g_fScale );
			g_SampleUI.GetStatic( IDC_PUFF_STATIC )->SetText( sz );
			metaballs.updateVolume(particlesContainer.getParticles(), particlesContainer.getParticlesCount(), g_fScale, g_fMetaballsSize);
			break;
		}
	case IDC_META_SCALE:
		{
			WCHAR sz[100];
			g_fMetaballsSize = ( float )( g_SampleUI.GetSlider( IDC_META_SCALE )->GetValue() * 0.01f );
			swprintf_s( sz, 100, L"Metaballs scale: %0.2f", g_fMetaballsSize );
			g_SampleUI.GetStatic( IDC_META_STATIC )->SetText( sz );
			metaballs.updateVolume(particlesContainer.getParticles(), particlesContainer.getParticlesCount(), g_fScale, g_fMetaballsSize);
			break;
		}
	}
}
