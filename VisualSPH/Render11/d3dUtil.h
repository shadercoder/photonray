#pragma once

// Let VC++ know we are compiling for WinXP and up.  This let's us use
// API functions specific to WinXP (e.g., WM_INPUT API).
#ifndef _WIN32_WINNT
#define _WIN32_WINNT   0x0600 // Vista
#endif

 
// Enable extra D3D debugging in debug builds if using the debug DirectX runtime.  
// This makes D3D objects work well in the debugger watch window, but slows down 
// performance slightly.
#if defined(DEBUG) || defined(_DEBUG)
	#ifndef D3D_DEBUG_INFO
	#define D3D_DEBUG_INFO
	#endif
#endif

#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "DXUT.h"
#include "SDKmisc.h"
#include <d3d11.h>
#include <d3d10.h>
#include <d3dx10.h>
#include <dxerr.h>
#include <cassert>
#include <atlbase.h>

//--------------------------------------------------------------------------------------
// Helper for creating constant buffers
//--------------------------------------------------------------------------------------
template <class T>
HRESULT CreateConstantBuffer(ID3D11Device* pd3dDevice, ID3D11Buffer** ppCB)
{
    HRESULT hr = S_OK;

    D3D11_BUFFER_DESC Desc;
    Desc.Usage = D3D11_USAGE_DEFAULT;
    Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    Desc.CPUAccessFlags = 0;
    Desc.MiscFlags = 0;
    Desc.ByteWidth = sizeof( T );
    V_RETURN( pd3dDevice->CreateBuffer( &Desc, NULL, ppCB ) );

    return hr;
}

//--------------------------------------------------------------------------------------
// Find and compile the specified shader
//--------------------------------------------------------------------------------------
static HRESULT CompileShaderFromFile( WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut )
{
    HRESULT hr = S_OK;

    // find the file
    WCHAR str[MAX_PATH];
    V_RETURN( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, szFileName ) );

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

    ID3DBlob* pErrorBlob;
    hr = D3DX11CompileFromFile( str, NULL, NULL, szEntryPoint, szShaderModel, 
        dwShaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL );
    if( FAILED(hr) )
    {
        if( pErrorBlob != NULL )
            OutputDebugStringA( (char*)pErrorBlob->GetBufferPointer() );
        SAFE_RELEASE( pErrorBlob );
        return hr;
    }
    SAFE_RELEASE( pErrorBlob );

    return S_OK;
}

//--------------------------------------------------------------------------------------
// Helper for creating structured buffers with an SRV and UAV
//--------------------------------------------------------------------------------------
template <class T>
HRESULT CreateStructuredBuffer(ID3D11Device* pd3dDevice, UINT iNumElements, ID3D11Buffer** ppBuffer, ID3D11ShaderResourceView** ppSRV, ID3D11UnorderedAccessView** ppUAV, const T* pInitialData = NULL)
{
    HRESULT hr = S_OK;

    // Create SB
    D3D11_BUFFER_DESC bufferDesc;
    ZeroMemory( &bufferDesc, sizeof(bufferDesc) );
    bufferDesc.ByteWidth = iNumElements * sizeof(T);
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
    bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    bufferDesc.StructureByteStride = sizeof(T);

    D3D11_SUBRESOURCE_DATA bufferInitData;
    ZeroMemory( &bufferInitData, sizeof(bufferInitData) );
    bufferInitData.pSysMem = pInitialData;
    V_RETURN( pd3dDevice->CreateBuffer( &bufferDesc, (pInitialData)? &bufferInitData : NULL, ppBuffer ) );

    // Create SRV
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ZeroMemory( &srvDesc, sizeof(srvDesc) );
    srvDesc.Format = DXGI_FORMAT_UNKNOWN;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    srvDesc.Buffer.ElementWidth = iNumElements;
    V_RETURN( pd3dDevice->CreateShaderResourceView( *ppBuffer, &srvDesc, ppSRV ) );

    // Create UAV
    D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
    ZeroMemory( &uavDesc, sizeof(uavDesc) );
    uavDesc.Format = DXGI_FORMAT_UNKNOWN;
    uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    uavDesc.Buffer.NumElements = iNumElements;
    V_RETURN( pd3dDevice->CreateUnorderedAccessView( *ppBuffer, &uavDesc, ppUAV ) );

    return hr;
}


//*****************************************************************************
// Simple d3d error checker 
//*****************************************************************************

#if defined(DEBUG) | defined(_DEBUG)
	#ifndef HR
	#define HR(x)                                              \
	{                                                          \
		HRESULT hr = (x);                                      \
		if(FAILED(hr))                                         \
		{                                                      \
			DXTrace(__FILE__, (DWORD)__LINE__, hr, L#x, true); \
		}                                                      \
	}
	#endif

#else
	#ifndef HR
	#define HR(x) (x)
	#endif
#endif 


//*****************************************************************************
// Convenience macro for releasing COM objects.
//*****************************************************************************

#define ReleaseCOM(x) { if(x){ x->Release();x = 0; } }


//*****************************************************************************
// Convenience functions.
//*****************************************************************************

// Converts ARGB 32-bit color format to ABGR 32-bit color format.
D3DX10INLINE UINT ARGB2ABGR(UINT argb)
{
	BYTE A = (argb >> 24) & 0xff;
	BYTE R = (argb >> 16) & 0xff;
	BYTE G = (argb >>  8) & 0xff;
	BYTE B = (argb >>  0) & 0xff;

	return (A << 24) | (B << 16) | (G << 8) | (R << 0);
}

// Returns random float in [0, 1).
D3DX10INLINE float RandF()
{
	return (float)(rand()) / (float)RAND_MAX;
}

// Returns random float in [a, b).
D3DX10INLINE float RandF(float a, float b)
{
	return a + RandF()*(b-a);
}

// Returns random vector on the unit sphere.
D3DX10INLINE D3DXVECTOR3 RandUnitVec3()
{
	D3DXVECTOR3 v(RandF(), RandF(), RandF());
	D3DXVec3Normalize(&v, &v);
	return v;
}
 
template<typename T>
D3DX10INLINE T Min(const T& a, const T& b)
{
	return a < b ? a : b;
}

template<typename T>
D3DX10INLINE T Max(const T& a, const T& b)
{
	return a > b ? a : b;
}
 
template<typename T>
D3DX10INLINE T Lerp(const T& a, const T& b, float t)
{
	return a + (b-a)*t;
}

template<typename T>
D3DX10INLINE T Clamp(const T& x, const T& low, const T& high)
{
	return x < low ? low : (x > high ? high : x); 
}


//*****************************************************************************
// Constants
//*****************************************************************************

const float INFINITY = FLT_MAX;
const float PI       = 3.14159265358979323f;
const float MATH_EPS = 0.0001f;

const D3DXCOLOR WHITE(1.0f, 1.0f, 1.0f, 1.0f);
const D3DXCOLOR BLACK(0.0f, 0.0f, 0.0f, 1.0f);
const D3DXCOLOR RED(1.0f, 0.0f, 0.0f, 1.0f);
const D3DXCOLOR GREEN(0.0f, 1.0f, 0.0f, 1.0f);
const D3DXCOLOR BLUE(0.0f, 0.0f, 1.0f, 1.0f);
const D3DXCOLOR YELLOW(1.0f, 1.0f, 0.0f, 1.0f);
const D3DXCOLOR CYAN(0.0f, 1.0f, 1.0f, 1.0f);
const D3DXCOLOR MAGENTA(1.0f, 0.0f, 1.0f, 1.0f);

const D3DXCOLOR BEACH_SAND(1.0f, 0.96f, 0.62f, 1.0f);
const D3DXCOLOR LIGHT_YELLOW_GREEN(0.48f, 0.77f, 0.46f, 1.0f);
const D3DXCOLOR DARK_YELLOW_GREEN(0.1f, 0.48f, 0.19f, 1.0f);
const D3DXCOLOR DARKBROWN(0.45f, 0.39f, 0.34f, 1.0f);

