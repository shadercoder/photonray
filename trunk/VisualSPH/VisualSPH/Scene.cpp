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

// Hotspot!
// Very slow reading from file by single value...
// TODO : implements buffering input
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

		current.presentation.vector.begin.x = x;
		current.presentation.vector.begin.y = y;
		current.presentation.vector.begin.z = z;
		current.presentation.position.x = x;
		current.presentation.position.y = y;
		current.presentation.position.z = z;

		current.presentation.vector.end.x = x + u * NORMAL_RATE;
		current.presentation.vector.end.y = y + v * NORMAL_RATE;
		current.presentation.vector.end.z = z + w * NORMAL_RATE;

		setGradientColorDensity(Ro, &current.presentation.position);

		current.presentation.vector.begin.color = D3DCOLOR_ARGB(255, 0, 255, 255);
		current.presentation.vector.end.color = D3DCOLOR_ARGB(255, 255, 0, 0);



		// TODO: unused parameters
		{
			float tmp;
			fin >> tmp;
		}
		Particles.push_back(current);
	}
	fin.close();
}

DWORD Scene::getColorGradient(float density)
{
	const int light_blue_r(0),light_blue_g(191), light_blue_b(255);
	const int blue_r(0),blue_g(0), blue_b(255);
	const int normal_denicity = 1000;
	int current_density = (int) density;
	int r,g,b;
	if(current_density<normal_denicity)
	{
		r =(normal_denicity-current_density)*255/1000;
		g =(current_density*(light_blue_g+255)/normal_denicity)-255;
		b = blue_b;
	}
	if(current_density>=normal_denicity&&current_density<normal_denicity*3)
	{
		r =0;
		g = ((current_density-normal_denicity*3)*light_blue_g)/((-2)*normal_denicity);
		b = blue_b;
	}
	if (current_density>=normal_denicity*3&&current_density<normal_denicity*5)
	{
		r=0;
		g=0;
		b = (255/((-2)*normal_denicity))*normal_denicity+((-5*normal_denicity)*255)/((-2)*normal_denicity);
	}
	if (current_density>=normal_denicity*5)
	{
		r=0;
		g=0;
		b=0;
	}
	DWORD result;
	
	result = D3DCOLOR_ARGB(255,r,g,b);
	return result;
}
void Scene::setGradientColorDensity(float density, Vertex* point)
{
	//TODO decomment
	point->color = getColorGradient(density);
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
	Vertex Vertexes[] = 
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
	if (FAILED(pDirect3DDevice->CreateVertexBuffer(4 * sizeof(Vertex), 0, D3DFVF_Vertex, D3DPOOL_DEFAULT, &pBufferVertexAxes, NULL)))
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
	pDirect3DDevice->SetStreamSource(0, pBufferVertexAxes, 0, sizeof(Vertex));
	pDirect3DDevice->SetFVF(D3DFVF_Vertex);
	pDirect3DDevice->SetIndices(pBufferIndex);
	pDirect3DDevice->DrawIndexedPrimitive(D3DPT_LINELIST, 0, 0, 4, 0, 3);
	pBufferVertexAxes->Release();
	pBufferIndex->Release();
	return S_OK;
}

//-----------------------------------------------------------------------------
//Name: DrawTexture
//------------------------------------------------------------------------------

HRESULT Scene::DrawTexture(Vertex& a,Vertex& b,Vertex& c,Vertex& d)
{
	LPDIRECT3DVERTEXBUFFER9 pBufferVertexBox	= NULL;;			// ������� ����� ��� ��������� ������
	LPDIRECT3DTEXTURE9		MyTexture;				// �������� ��� ������ ������
	Vertex Vertexes[] ={a,
						b,
						c,
						
						c,
						d,
						a,};
			
	/*			{ -1.0f, -1.0f,  1.0f, D3DCOLOR_XRGB( 0,   20,  100), 0.0f, 0.0f,},
		{  1.0f, -1.0f,  1.0f, D3DCOLOR_XRGB( 0,   20,  100), 1.0f, 0.0f,},
		{  1.0f,  1.0f,  1.0f, D3DCOLOR_XRGB( 0,   20,  100), 1.0f, 1.0f,}, 
		{ -1.0f, -1.0f,  1.0f, D3DCOLOR_XRGB( 0,   20,  100), 0.0f, 0.0f,},
		{  1.0f,  1.0f,  1.0f, D3DCOLOR_XRGB( 0,   20,  100), 1.0f, 1.0f,}, 
		{ -1.0f,  1.0f,  1.0f, D3DCOLOR_XRGB( 0,   20,  100), 0.0f, 1.0f,}, 
*/

	if (FAILED(pDirect3DDevice->CreateVertexBuffer(6 * sizeof(Vertex), 0, D3DFVF_Vertex, D3DPOOL_DEFAULT, &pBufferVertexBox, NULL)))
		return E_FAIL;
void * pBuf;
	// ����������� ��������� �����
	pBufferVertexBox->Lock( 0, 6 * sizeof(Vertex), &pBuf, 0 );
	memcpy( pBuf, Vertexes, 36 * sizeof(Vertex));
	pBufferVertexBox->Unlock();

	// ��������� �������� �� �����
	D3DXCreateTextureFromFile(pDirect3DDevice, "index.jpg", &MyTexture);


		// ������������� ��������� �����
		pDirect3DDevice->SetStreamSource( 0,pBufferVertexBox, 0, sizeof(Vertex) );
		// ������������� ������ ������
		pDirect3DDevice->SetFVF( D3DFVF_Vertex );

		// ���� �������� ��������� �����������
		if( MyTexture != NULL )
			pDirect3DDevice->SetTexture(0, MyTexture); // ������ ��������
		pDirect3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		pDirect3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		pDirect3DDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

		// ���������� ���������
		pDirect3DDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 2);


		pDirect3DDevice->EndScene();							// ��������� ��������
		pDirect3DDevice->Present( NULL, NULL, NULL, NULL );	// ���������� ������ ����� �� �����


	return S_OK;


}

//-----------------------------------------------------------------------------
// Name: DrawBox()
//-----------------------------------------------------------------------------
HRESULT Scene::DrawBox(float xmin=-1, float ymin=-1, float zmin=0, float xmax=1, float ymax=3, float zmax=2)
{	


	LPDIRECT3DVERTEXBUFFER9 pBufferVertexBox	= NULL;
	LPDIRECT3DINDEXBUFFER9 pBufferIndex			= NULL;

	Vertex vert_text[4] ={
		{  xmin,ymin,zmin,D3DCOLOR_ARGB(255, 255, 255, 255),0.0f,0.0f },
		{  xmax,ymin,zmin,D3DCOLOR_ARGB(255, 255, 255, 255),1.0f,0.0f },
		{  xmax,ymin,zmax,D3DCOLOR_ARGB(255, 255, 255, 255),1.0f,1.0f },
		{  xmin,ymin,zmax,D3DCOLOR_ARGB(255, 255, 255, 255),0.0f,1.0f },
	};

	DrawTexture(vert_text[0],vert_text[1],vert_text[2],vert_text[3]);

	Vertex Vertexes[] = 
	{
       {  xmin,ymin,zmin,D3DCOLOR_ARGB(255, 255, 255, 255), }, //�
       {  xmax,ymin,zmin,D3DCOLOR_ARGB(255, 255, 255, 255), }, //�
       {  xmin, ymax,zmin, D3DCOLOR_ARGB(255, 255, 255, 255), }, //�
	   {  xmin, ymin,zmax,D3DCOLOR_ARGB(255, 255, 255, 255), }, //D


	   {  xmax,ymax,zmin,D3DCOLOR_ARGB(255, 255, 255, 255), }, //�
       {  xmin,ymax,zmin,D3DCOLOR_ARGB(255, 255, 255, 255), }, //�
       {  xmax, ymin,zmin, D3DCOLOR_ARGB(255, 255, 255, 255), }, //�
	   {  xmax, ymax,zmax,D3DCOLOR_ARGB(255, 255, 255, 255), }, //D
        
      
             
       { xmin,ymax,zmax,D3DCOLOR_ARGB(255, 255, 255, 255), }, //�
       { xmax,ymax,zmax,D3DCOLOR_ARGB(255, 255, 255, 255), }, //�
       { xmin, ymin,zmax, D3DCOLOR_ARGB(255, 255, 255, 255), }, //�
	   { xmin, ymax,zmin,D3DCOLOR_ARGB(255, 255, 255, 255), }, //D 


            
       { xmax,ymin,zmax,D3DCOLOR_ARGB(255, 255, 255, 255), }, //�
       { xmin,ymin,zmax,D3DCOLOR_ARGB(255, 255, 255, 255), }, //�
       { xmax,ymax,zmax, D3DCOLOR_ARGB(255, 255, 255, 255), }, //�
	   { xmax, ymin,zmin,D3DCOLOR_ARGB(255, 255, 255, 255), }, //D
    
    };
    const unsigned short Index[]={
   	
		0, 1,
		0, 2,
		0, 3,

	
		4,5,
		4,6,
		4,7,


		8,9,
		8,10,
		8,11,

		
		12,13,
		12,14,
		12,15

	};


	if (FAILED(pDirect3DDevice->CreateVertexBuffer(24 * sizeof(Vertex), 0, D3DFVF_Vertex, D3DPOOL_DEFAULT, &pBufferVertexBox, NULL)))
		return E_FAIL;
	VOID* pBV;
	if(FAILED(pBufferVertexBox->Lock(0, sizeof(Vertexes), (void**) &pBV, 0)))
		return E_FAIL;
	memcpy(pBV, Vertexes, sizeof(Vertexes));
	pBufferVertexBox->Unlock();

	pDirect3DDevice->CreateIndexBuffer(24 * sizeof(Index), 0, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &pBufferIndex, NULL);
	VOID* pBI;
	pBufferIndex->Lock(0, sizeof(Index), (void**) &pBI, 0);
	memcpy(pBI, Index, sizeof(Index));
	pBufferIndex->Unlock();
	
		
	pDirect3DDevice->SetStreamSource(0, pBufferVertexBox, 0, sizeof(Vertex));
	pDirect3DDevice->SetFVF(D3DFVF_Vertex);
	pDirect3DDevice->SetIndices(pBufferIndex);


	pDirect3DDevice->DrawIndexedPrimitive(D3DPT_LINELIST, 0, 0, 4, 0, 3);
	pDirect3DDevice->DrawIndexedPrimitive(D3DPT_LINELIST, 4, 0, 4, 0, 3);
	pDirect3DDevice->DrawIndexedPrimitive(D3DPT_LINELIST, 8, 0, 4, 0, 3);
	pDirect3DDevice->DrawIndexedPrimitive(D3DPT_LINELIST, 12, 0, 4, 0, 3);
	pBufferIndex->Release();
	
	
	return S_OK;
}



VOID Scene::TranslateParticles()
{
	D3DXMATRIX MatrixWorld;
	D3DXMATRIX MatrixWorldTranslate;
	D3DXMATRIX MatrixWorldRotate;
	D3DXMATRIX MatrixWorldScale;
	D3DXMatrixTranslation(&MatrixWorldTranslate, 0.0f, -1.0f, 0.0f);
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
	pDirect3DDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB( 42, 42, 42 ), 1.0f, 0 );

	// Begin the scene
	if( SUCCEEDED( pDirect3DDevice->BeginScene() ) )
	{

		// Rendering of scene objects can happen here
		// Axes
		D3DXMATRIX MatrixWorld;
		D3DXMatrixIdentity( &MatrixWorld );
		pDirect3DDevice->SetTransform(D3DTS_WORLD, &MatrixWorld);
		DrawBox(-1,-1,-1,1,1,1);
		
		DrawAxes();

		// Translate particles
		TranslateParticles();	
		ParticleRender();
		//ParticleVelocityRender();
		//ParticleDensityRender();

		//End the scene
		pDirect3DDevice->EndScene();
	}

	// Present the backbuffer contents to the display
	pDirect3DDevice->Present( NULL, NULL, NULL, NULL );
}

// Render particle as a simple point
HRESULT Scene::ParticleRender()
{	
	LPDIRECT3DVERTEXBUFFER9 pBufferParticles = NULL;
	if(Particles.size() == 0) 
	{
		// Nothing to draw
		return S_OK;
	}	
	// Setup vertex buffer for available particles
	if (FAILED(pDirect3DDevice->CreateVertexBuffer(Particles.size() * sizeof(ParticleVertex), 0, D3DFVF_PARTICLE, D3DPOOL_DEFAULT, &pBufferParticles, NULL)))
		return E_FAIL;
	ParticleVertex* pBV;
	if(FAILED(pBufferParticles->Lock(0, Particles.size() * sizeof(ParticleVertex), (void**) &pBV, 0)))
		return E_FAIL;
	// Copy particles to vertex buffer
	//memcpy(pBV, (void*)&Particles[0], Particles.size() * sizeof(ParticleVertex));
	for (int i = 0; i < Particles.size(); ++i)
	{
		pBV[i] = Particles[i].presentation.position;
	}
	pBufferParticles->Unlock();
	// Render
	pDirect3DDevice->SetStreamSource(0, pBufferParticles, 0, sizeof(ParticleVertex));
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
	if (FAILED(pDirect3DDevice->CreateVertexBuffer(Particles.size() * sizeof(Particle), 0, D3DFVF_Vertex, D3DPOOL_DEFAULT, &pBufferParticles, NULL)))
		return E_FAIL;
	VOID* pBV;
	if(FAILED(pBufferParticles->Lock(0, sizeof(Particles), (void**) &pBV, 0)))
		return E_FAIL;
	// Copy particles to vertex buffer
	memcpy(pBV, (void*)&Particles[0], Particles.size() * sizeof(Particle));
	pBufferParticles->Unlock();
	// Render
	pDirect3DDevice->SetStreamSource(0, pBufferParticles, 0, sizeof(Particle));
	pDirect3DDevice->SetFVF(D3DFVF_Vertex);
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
	if (FAILED(pDirect3DDevice->CreateVertexBuffer(2 * Particles.size() * sizeof(Vertex), 0, D3DFVF_Vertex, D3DPOOL_DEFAULT, &pBufferParticles, NULL)))
		return E_FAIL;

	Vertex* pBV;
	if(FAILED(pBufferParticles->Lock(0, sizeof(Vertex), (void**) &pBV, 0)))
		return E_FAIL;

	int j = 0; 
	for(int i = 0; i < Particles.size() * 2; i += 2)
	{
		pBV[i] = Particles[j].presentation.vector.begin;
		pBV[i+1] = Particles[j].presentation.vector.end;
		j++;
	}
	pBufferParticles->Unlock();

	// Render
	pDirect3DDevice->SetStreamSource(0, pBufferParticles, 0, sizeof(Vertex));
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

	sprintf(filename, "./screenshots/frame_%d.png", frame);        


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