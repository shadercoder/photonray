#include <cstdlib>
#include <crtdbg.h>
#include <sstream>

#include "common\d3dApp.h"
#include "Common\Effects.h"
#include "Box.h"
#include "Axis.h"
#include "InputStreamPS.h"
#include "Settings.h"
#include "HUD.h"
#include "Common\Camera.h"
#include "Common\TextureMgr.h"

class VisualSPH : public D3DApp
{
public:
	VisualSPH(HINSTANCE hInstance);
	~VisualSPH();
	
	Settings settings;
	HUD hud;
	// primitives
	Box boundingBox;
	Axis axis;
	InputStreamPS particleSystem;

	void initApp();
	void onResize();
	void updateScene(float dt);
	void drawScene(); 

private:
	void buildFX();
	void buildVertexLayouts();
	HRESULT msgProc(UINT msg, WPARAM wParam, LPARAM lParam);
private:
	POINT mOldMousePos;
	ID3D10Effect* mFX;
	ID3D10EffectTechnique* mTech;
	ID3D10InputLayout* mVertexLayout;
	ID3D10EffectMatrixVariable* mfxWVPVar;
	ID3D10EffectMatrixVariable* g_pmInvView;

	D3DXMATRIX mView;
	D3DXMATRIX mProj;
	D3DXMATRIX mWVP;

	float mTheta;
	float mPhi;
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
				   PSTR cmdLine, int showCmd)
{
	//_CrtSetBreakAlloc(155);
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif


	VisualSPH theApp(hInstance);
	
	theApp.initApp();

	return theApp.run();
}

VisualSPH::VisualSPH(HINSTANCE hInstance)
: D3DApp(hInstance), mFX(0), mTech(0), mVertexLayout(0),
  mfxWVPVar(0), mTheta(0.0f), mPhi(PI*0.25f)
{
	D3DXMatrixIdentity(&mView);
	D3DXMatrixIdentity(&mProj);
	D3DXMatrixIdentity(&mWVP); 
}

VisualSPH::~VisualSPH()
{
	if( md3dDevice )
		md3dDevice->ClearState();

	ReleaseCOM(mFX);
	ReleaseCOM(mVertexLayout);
}

void VisualSPH::initApp()
{
	this->mMainWndCaption = L"[Visual SPH]";
	D3DApp::initApp();

	hud.init(md3dDevice);
	// init settings
	settings.loadFromFile("settings.txt");
	// Camera 
	GetCamera().setPosition() = settings.cameraPos;
	GetCamera().setLook() = (settings.cameraLookAt - settings.cameraPos);
	GetCamera().setUp() = D3DXVECTOR3(0.f, 1.f, 0.f);
	
	particleSystem.init(md3dDevice, settings.pathToFolder, settings.patternString, settings.firstFrame, settings.lastFrame, settings.stepFrame);
	// TODO: fix load initial frame 
	particleSystem.getFrame(settings.firstFrame);
	boundingBox.init(md3dDevice, D3D10_PRIMITIVE_TOPOLOGY_LINELIST, D3DXVECTOR3(1.1f,1.1f,3.0f));
	axis.init(md3dDevice);
	GetTextureMgr().init(md3dDevice);
	GetTextureMgr().createTex(L"Particle.dds");
	buildFX();
	buildVertexLayouts();
	
}

void VisualSPH::onResize()
{
	D3DApp::onResize();

	float aspect = (float)mClientWidth/mClientHeight;
	D3DXMatrixPerspectiveFovLH(&mProj, 0.25f*PI, aspect, 1.0f, 1000.0f);
}

void VisualSPH::updateScene(float dt)
{
	//D3DApp::updateScene(dt);
	
	// Code computes the average frames per second, and also the 
	// average time it takes to render one frame.

	static int frameCnt = 0;
	static float t_base = 0.0f;

	frameCnt++;

	// Compute averages over one second period.
	if( (mTimer.getGameTime() - t_base) >= 0.25f )
	{
		float fps = (float)frameCnt; // fps = frameCnt / 1
		float mspf = 1000.0f / fps;
		// TODO: conver to normal setFPS, setFRAME
		std::wostringstream outs;   
		outs.precision(6);
		outs << "FPS: " << fps << "\n" 
			 << "Milliseconds: Per Frame: " << mspf << "\n"
			 << "Frame: " << this->particleSystem.getNumCurrFrame() << endl;
		hud.setText(outs.str());
		//mFrameStats = std::wstring( outs.str());
		// Reset for next average.
		frameCnt = 0;
		t_base  += 1.0f;
	}

	// Update angles based on input to orbit camera around box.

	// Update angles based on input to orbit camera around scene.
	if(GetAsyncKeyState('A') & 0x8000)	GetCamera().strafe(-1.3f*dt);
	if(GetAsyncKeyState('D') & 0x8000)	GetCamera().strafe(+1.3f*dt);
	if(GetAsyncKeyState('W') & 0x8000)	GetCamera().walk(+1.3f*dt);
	if(GetAsyncKeyState('S') & 0x8000)	GetCamera().walk(-1.3f*dt);
 
	GetCamera().rebuildView();

	// TODO: fix auto switch next frame
	if(GetAsyncKeyState('N') & 0x8000)	particleSystem.getNextFrame();
	// TODO: fix to correct exit by press esc button
	if(GetAsyncKeyState(27) & 0x8000) exit(0);

	
}

void VisualSPH::drawScene()
{
	D3DApp::drawScene();

	// Restore default states, input layout and primitive topology 
	// because mFont->DrawText changes them.  Note that we can 
	// restore the default states by passing null.
	md3dDevice->OMSetDepthStencilState(0, 0);
	float blendFactors[] = {0.0f, 0.0f, 0.0f, 0.0f};
	md3dDevice->OMSetBlendState(0, blendFactors, 0xffffffff);
    md3dDevice->IASetInputLayout(mVertexLayout);
    //md3dDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//md3dDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINELIST);

   
	// set constants
	mWVP = GetCamera().view()*mProj;
	mfxWVPVar->SetMatrix((float*)&mWVP);

	D3DXMATRIX mInvView;
    D3DXMatrixInverse( &mInvView, NULL, &GetCamera().view() );
    g_pmInvView->SetMatrix( ( float* )&mInvView );


	md3dDevice->RSSetState(0); // restore default


    D3D10_TECHNIQUE_DESC techDesc;
	mTech = mFX->GetTechniqueByName("Render");
    mTech->GetDesc( &techDesc );
    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
        mTech->GetPassByIndex( p )->Apply(0);		
		boundingBox.draw();
		axis.draw();
	}

	mTech = mFX->GetTechniqueByName("RenderParticles");
    mTech->GetDesc( &techDesc );
    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
        mTech->GetPassByIndex( p )->Apply(0);		
		particleSystem.drawAll();
	}


	hud.draw();
	
	mSwapChain->Present(0, 0);
}

void VisualSPH::buildFX()
{
	DWORD shaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
    shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif
 
	ID3D10Blob* compilationErrors = 0;
	HRESULT hr = 0;
	hr = D3DX10CreateEffectFromFile(L"color.fx", 0, 0, 
		"fx_4_0", shaderFlags, 0, md3dDevice, 0, 0, &mFX, &compilationErrors, 0);
	if(FAILED(hr))
	{
		if( compilationErrors )
		{
			MessageBoxA(0, (char*)compilationErrors->GetBufferPointer(), 0, 0);
			ReleaseCOM(compilationErrors);
		}
		DXTrace(__FILE__, (DWORD)__LINE__, hr, L"D3DX10CreateEffectFromFile", true);
	} 

	mTech = mFX->GetTechniqueByName("Render");
	
	mfxWVPVar = mFX->GetVariableByName("gWVP")->AsMatrix();
	g_pmInvView = mFX->GetVariableByName( "g_mInvView" )->AsMatrix();
	mFX->GetVariableByName("g_txParticle")->AsShaderResource()->SetResource(GetTextureMgr().createTex(L"Particle.dds"));
}

void VisualSPH::buildVertexLayouts()
{
	// Create the vertex input layout.
	D3D10_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  D3D10_APPEND_ALIGNED_ELEMENT, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,  D3D10_APPEND_ALIGNED_ELEMENT, D3D10_INPUT_PER_VERTEX_DATA, 0},		
	};
	// Create the input layout
    D3D10_PASS_DESC PassDesc;
    mTech->GetPassByIndex(0)->GetDesc(&PassDesc);
    HR(md3dDevice->CreateInputLayout(vertexDesc, 3, PassDesc.pIAInputSignature,
		PassDesc.IAInputSignatureSize, &mVertexLayout));
}

LRESULT VisualSPH::msgProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
	POINT mousePos;
	int dx = 0;
	int dy = 0;
	switch(msg)
	{
	case WM_LBUTTONDOWN:
		if( wParam & MK_LBUTTON )
		{
			SetCapture(mhMainWnd);

			mOldMousePos.x = LOWORD(lParam);
			mOldMousePos.y = HIWORD(lParam);
		}
		return 0;

	case WM_LBUTTONUP:
		ReleaseCapture();
		return 0;

	case WM_MOUSEMOVE:
		if( wParam & MK_LBUTTON )
		{
			mousePos.x = (int)LOWORD(lParam); 
			mousePos.y = (int)HIWORD(lParam); 
			dx = mousePos.x - mOldMousePos.x;
			dy = mousePos.y - mOldMousePos.y;

			GetCamera().pitch( dy * 0.0087266f );
			GetCamera().rotateY( dx * 0.0087266f );
			
			mOldMousePos = mousePos;
		}
		return 0;
	}

	return D3DApp::msgProc(msg, wParam, lParam);
}
