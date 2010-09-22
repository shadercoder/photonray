#include "Scene.h"


Scene::Scene(void): WINDOWED (TRUE), SCREEN_WIDTH (800), SCREEN_HEIGHT(600)
{

}


Scene::~Scene(void)
{
	if( pDirect3DDevice != NULL )
		pDirect3DDevice->Release();

	if( pDirect3D != NULL )
		pDirect3D->Release();
}

VOID Scene::InputParticles(const int frame)
{
	Particles.clear();
	//string filename = "./frames/2/";
	string filename = pathToFolder;
	char numfile[5] = {};
	sprintf(numfile, "%d", frame);
	for (int i = 0; i < 5 - strlen(numfile); ++i)
	{
		filename.append("0");
	}
	filename.append(numfile);
	//filename.append("_flu.txt");
	filename.append(filePattern);
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

//-----------------------------------------------------------------------------
// Name: DrawAxes()
// Desc: Draws the axes x, y, z
//-----------------------------------------------------------------------------
HRESULT Scene::DrawAxes()
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
VOID Scene::TranslateParticles()
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
// Name: Render()
// Desc: Draws the scene
//-----------------------------------------------------------------------------
VOID Scene::Render()
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
		TranslateParticles();	
		ParticleRender();

		//End the scene
		pDirect3DDevice->EndScene();
		//TakeScreenShot();
	}

	// Present the backbuffer contents to the display
	pDirect3DDevice->Present( NULL, NULL, NULL, NULL );
}

// Particle render
HRESULT Scene::ParticleRender()
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

void Scene::setView()
{
	cam.setView(pDirect3DDevice);
}

HRESULT Scene::TakeScreenShot(const int frame, HWND hWnd)
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

void Scene::setParameter(string pathToFolder, string filePattern, int firstFrame, int lastFrame, int step, float camX, float camY, float camZ, float lookX, float lookY, float lookZ)
{
	this->pathToFolder = pathToFolder;
	this->filePattern = filePattern;
	this->firstFrame = firstFrame;
	this->lastFrame = lastFrame;
	this->step = step;
	D3DXVECTOR3 pos(camX, camY, camZ);
	cam.setPosition(&pos);
	D3DXVECTOR3 look(lookX, lookY, lookZ);
	cam.setLook(&look);
}
