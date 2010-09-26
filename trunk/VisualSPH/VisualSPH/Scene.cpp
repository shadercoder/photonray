#include "Scene.h"


Scene::Scene(void): WINDOWED (TRUE), SCREEN_WIDTH (800), SCREEN_HEIGHT(600), DRAW_SCALE (2.0f), NORMAL_RATE(0.05f)
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
	string filename = pathToFolder;
	char numfile[5] = {};
	sprintf(numfile, "%d", frame);
	for (int i = 0; i < 5 - strlen(numfile); ++i)
	{
		filename.append("0");
	}
	filename.append(numfile);
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
		float x, y, z, u, v, w, Ro;
		Particle current;
		fin >> x >> y >> z >> u >> v >> w >> Ro;
		
		current.x0.x = x;
		current.x0.y = y;
		current.x0.z = z;
		current.pos = current.x0;

		current.x1.x = x + u * NORMAL_RATE;
		current.x1.y = y + v * NORMAL_RATE;
		current.x1.z = z + w * NORMAL_RATE;

		setGradientColorDensity(Ro, &current.pos);

		current.x0.color = D3DCOLOR_ARGB(255, 0, 255, 255);
		current.x1.color = D3DCOLOR_ARGB(255, 255, 0, 0);
		// TODO: unused parameters
		for (int j = 0; j < 1; ++j) 
		{
			float tmp;
			fin >> tmp;
		}
		Particles.push_back(current);
	}
	fin.close();
}

DWORD Scene::getGradientColorFromTo(DWORD color1, DWORD color2, float value)
{
	DWORD a1 = (color1>>24)&0xFF;
	DWORD r1 = (color1>>16)&0xFF;
	DWORD g1 = (color1>>8)&0xFF;
	DWORD b1 = (color1>>32)&0xFF;
	DWORD a2 = (color2>>24)&0xFF;
	DWORD r2 = (color2>>16)&0xFF;
	DWORD g2 = (color2>>8)&0xFF;
	DWORD b2 = (color2>>32)&0xFF;
	float aDif,rDif,gDif,bDif;
	aDif = ((float)a2-(float)a1)/value;
	rDif = ((float)r2-(float)r1)/value;
	gDif = ((float)g2-(int)g1)/value;
	bDif = ((float)b2-(float)b1)/value;
	DWORD DrawCol;
	//DrawCol = (a1+(int)(aDif*value) << 24) | (r1+(int)(rDif * value) << 16) | (g1+(int)(gDif * value) << 8) | (b1+(int)(bDif*value));
	DrawCol = (a1+(int)(aDif) << 24) | (r1+(int)(rDif) << 16) | (g1+(int)(gDif) << 8) | (b1+(int)(bDif));
	return DrawCol;
}

void Scene::setGradientColorDensity(float density, CUSTOMVERTEX* point)
{

	if (density > 2000 && density < 2500)
	{
		// from yellow to red		
		point->color = getGradientColorFromTo(D3DCOLOR_ARGB(255, 255, 237, 0), D3DCOLOR_ARGB(255, 255, 127, 80), density);
	}
	if (density > 1500 && density < 2000)
	{
		// from green to yellow
		point->color = getGradientColorFromTo(D3DCOLOR_ARGB(255, 127, 255, 0), D3DCOLOR_ARGB(255, 255, 237, 0), density);
	}
	if (density > 1000 && density < 1500)
	{
		// from blue to green
		point->color = getGradientColorFromTo(D3DCOLOR_ARGB(255, 28, 57, 187), D3DCOLOR_ARGB(255, 127, 255, 0), density);
	}
	if (density < 1000)
	{
		// from violet to blue
		point->color = getGradientColorFromTo(D3DCOLOR_ARGB(255, 128, 0, 255), D3DCOLOR_ARGB(255, 28, 57, 187), density);
	}
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
	D3DXMatrixScaling(&MatrixWorldScale, DRAW_SCALE, DRAW_SCALE, DRAW_SCALE);
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
		//ParticleRender();
		ParticleVelocityRender();
		//ParticleDensityRender();

		//End the scene
		pDirect3DDevice->EndScene();
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

// Particle render
HRESULT Scene::ParticleDensityRender()
{	
	LPDIRECT3DVERTEXBUFFER9 pBufferParticles = NULL;
	if(Particles.size() == 0) 
	{
		// Nothing to draw
		return S_OK;
	}	
	// Setup vertex buffer for available particles
	if (FAILED(pDirect3DDevice->CreateVertexBuffer(Particles.size() * sizeof(Particle), 0, D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &pBufferParticles, NULL)))
		return E_FAIL;
	VOID* pBV;
	if(FAILED(pBufferParticles->Lock(0, sizeof(Particles), (void**) &pBV, 0)))
		return E_FAIL;
	// Copy particles to vertex buffer
	memcpy(pBV, (void*)&Particles[0], Particles.size() * sizeof(Particle));
	pBufferParticles->Unlock();
	// Render
	pDirect3DDevice->SetStreamSource(0, pBufferParticles, 0, sizeof(Particle));
	pDirect3DDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
	pDirect3DDevice->DrawPrimitive(D3DPT_POINTLIST, 0, Particles.size());
	pBufferParticles->Release();
	return S_OK;
}


HRESULT Scene::ParticleVelocityRender()
{	
	LPDIRECT3DVERTEXBUFFER9 pBufferParticles = NULL;
	if(Particles.size() == 0) 
	{
		// Nothing to draw
		return S_OK;
	}	
	// Setup vertex buffer for available particles
	if (FAILED(pDirect3DDevice->CreateVertexBuffer(2 * Particles.size() * sizeof(CUSTOMVERTEX), 0, D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &pBufferParticles, NULL)))
		return E_FAIL;

	CUSTOMVERTEX* pBV = new CUSTOMVERTEX[Particles.size() * 2];
	if(FAILED(pBufferParticles->Lock(0, sizeof(CUSTOMVERTEX), (void**) &pBV, 0)))
		return E_FAIL;

	int j=0; 
	for(int i=0; i<Particles.size()*2; i+=2)
	{
		pBV[i] = Particles[j].x0;
		pBV[i+1] = Particles[j].x1;
		j++;
	}
	pBufferParticles->Unlock();

	// Render
	pDirect3DDevice->SetStreamSource(0, pBufferParticles, 0, sizeof(CUSTOMVERTEX));
	pDirect3DDevice->SetFVF(D3DFVF_PARTICLE);
	pDirect3DDevice->DrawPrimitive(D3DPT_POINTLIST, 0, 2*Particles.size());
	pDirect3DDevice->DrawPrimitive(D3DPT_LINELIST, 0, Particles.size());
	pBufferParticles->Release();
	return S_OK;
}

void Scene::setView()
{
	cam.setView(pDirect3DDevice);
}

HRESULT Scene::TakeScreenShot(const int frame)
{    
	HRESULT hr = S_OK;    
	LPDIRECT3DSURFACE9 frontbuf;    
	char filename[64] = {};        
	int x, y;    
	RECT rcWindow;     

	sprintf(filename, "./screenshots/frame_%d.bmp", frame);        


	// get the screen resolution. If it's a windowed application get the whole    
	// screen size. If it's a fullscreen application you might have somewhere     
	// your defines as: 
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
		if(!GetWindowRect( *hWnd, &rcWindow ))
		{
			log.errors(string("GetWindowRect( *hWnd, &rcWindow )"));
		}
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
	{    
		//if(!SUCCEEDED( D3DXSaveSurfaceToFile( filename, D3DXIFF_BMP, frontbuf, NULL, NULL)))
		if(!SUCCEEDED( D3DXSaveSurfaceToFile( filename, D3DXIFF_PNG, frontbuf, NULL, NULL)))
		{
			log.errors(string("Couldn't save screenshot"));
		}
	}
	else
	{
		//D3DXSaveSurfaceToFile( filename, D3DXIFF_JPG, frontbuf, NULL, &rcWindow);     
		//if(!SUCCEEDED( D3DXSaveSurfaceToFile( filename, D3DXIFF_BMP, frontbuf, NULL, &rcWindow)))
		if(!SUCCEEDED( D3DXSaveSurfaceToFile( filename, D3DXIFF_PNG, frontbuf, NULL, &rcWindow)))
		{
			log.errors(string("Couldn't save screenshot"));
		}
	}

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

void Scene::setHWND(HWND& window)
{
	this->hWnd = &window;
}