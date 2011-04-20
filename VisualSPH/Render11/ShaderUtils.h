#pragma once
#include "DXUT.h"
#include "d3dUtil.h"
#include <cstdio>

HRESULT CreatePixelShaderFromAssembly(ID3D10Device* device, const char* filename, ID3D10PixelShader** ppPixelShader)
{
	HRESULT hr = S_OK;
	const int MAX_BUFF = 1 << 20;
	const char* shaderCode = new char[MAX_BUFF];
	FILE* in;
	fopen_s(&in, filename, "rb+");
	size_t shaderSize = fread_s((void*) shaderCode, MAX_BUFF, sizeof(char), MAX_BUFF, in);
	hr = device->CreatePixelShader(shaderCode, shaderSize, ppPixelShader);
	fclose(in);
	delete [] shaderCode;
	return hr;
}

HRESULT CreateVertexShaderAndInputLayoutFromAssembly(ID3D10Device* device, const char* filename, ID3D10VertexShader** ppVertexShader, D3D10_INPUT_ELEMENT_DESC* pInputLayoutDesc, UINT numElements, ID3D10InputLayout** ppInputLayout)
{
	HRESULT hr = S_OK;
	const int MAX_BUFF = 1 << 20;
	const char* shaderCode = new char[MAX_BUFF];
	FILE* in;
	fopen_s(&in, filename, "rb+");
	size_t shaderSize = fread_s((void*) shaderCode, MAX_BUFF, sizeof(char), MAX_BUFF, in);
	hr = device->CreateVertexShader(shaderCode, shaderSize, ppVertexShader);	
	hr = device->CreateInputLayout(pInputLayoutDesc, numElements, shaderCode, shaderSize, ppInputLayout);
	fclose(in);
	delete [] shaderCode;
	return hr;
}
