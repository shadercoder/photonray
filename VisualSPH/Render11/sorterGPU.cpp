#include "DXUT.h"
#include "sorterGPU.h"


sorterGPU::sorterGPU(void)
{
}


sorterGPU::~sorterGPU(void)
{
}


HRESULT sorterGPU::updateBuffer(void* newData)
{
	data = newData;
	return S_OK;
}

//--------------------------------------------------------------------------------------
// Create the Resources
//--------------------------------------------------------------------------------------
HRESULT sorterGPU::CreateResources()
{
    HRESULT hr = S_OK;
    ID3DBlob* pBlob = NULL;

    // Compile the Bitonic Sort Compute Shader
    hr = CompileShaderFromFile( L"ComputeShaderSort11.hlsl", "BitonicSort", "cs_4_0", &pBlob );
    if( FAILED( hr ) )
        return hr;

    // Create the Bitonic Sort Compute Shader
    hr = g_pd3dDevice->CreateComputeShader( pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &g_pComputeShaderBitonic );
    if( FAILED( hr ) )
        return hr;
    SAFE_RELEASE( pBlob );
#if defined(DEBUG) || defined(PROFILE)
    g_pComputeShaderBitonic->SetPrivateData( WKPDID_D3DDebugObjectName, sizeof( "BitonicSort" ) - 1, "BitonicSort" );
#endif

    // Compile the Matrix Transpose Compute Shader
    hr = CompileShaderFromFile( L"ComputeShaderSort11.hlsl", "MatrixTranspose", "cs_4_0", &pBlob );
    if( FAILED( hr ) )
        return hr;

    // Create the Matrix Transpose Compute Shader
    hr = g_pd3dDevice->CreateComputeShader( pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &g_pComputeShaderTranspose );
    if( FAILED( hr ) )
        return hr;
    SAFE_RELEASE( pBlob );
#if defined(DEBUG) || defined(PROFILE)
    g_pComputeShaderTranspose->SetPrivateData( WKPDID_D3DDebugObjectName, sizeof( "MatrixTranspose" ) - 1, "MatrixTranspose" );
#endif

    // Create the Const Buffer
    D3D11_BUFFER_DESC constant_buffer_desc;
    ZeroMemory( &constant_buffer_desc, sizeof(constant_buffer_desc) );
    constant_buffer_desc.ByteWidth = sizeof(CB);
    constant_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    constant_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    constant_buffer_desc.CPUAccessFlags = 0;
    hr = g_pd3dDevice->CreateBuffer( &constant_buffer_desc, NULL, &g_pCB );
    if( FAILED( hr ) )
        return hr;
#if defined(DEBUG) || defined(PROFILE)
    g_pCB->SetPrivateData( WKPDID_D3DDebugObjectName, sizeof( "CB" ) - 1, "CB" );
#endif

    // Create the Buffer of Elements
    // Create 2 buffers for switching between when performing the transpose
    D3D11_BUFFER_DESC buffer_desc;
    ZeroMemory( &buffer_desc, sizeof(buffer_desc) );
    buffer_desc.ByteWidth = NUM_ELEMENTS * sizeof(UINT);
    buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    buffer_desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
    buffer_desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    buffer_desc.StructureByteStride = sizeof(UINT);
    hr = g_pd3dDevice->CreateBuffer( &buffer_desc, NULL, &g_pBuffer1 );
    if( FAILED( hr ) )
        return hr;
#if defined(DEBUG) || defined(PROFILE)
    g_pBuffer1->SetPrivateData( WKPDID_D3DDebugObjectName, sizeof( "Buffer1" ) - 1, "Buffer1" );
#endif

    hr = g_pd3dDevice->CreateBuffer( &buffer_desc, NULL, &g_pBuffer2 );
    if( FAILED( hr ) )
        return hr;
#if defined(DEBUG) || defined(PROFILE)
    g_pBuffer2->SetPrivateData( WKPDID_D3DDebugObjectName, sizeof( "Buffer2" ) - 1, "Buffer2" );
#endif

    // Create the Shader Resource View for the Buffers
    // This is used for reading the buffer during the transpose
    D3D11_SHADER_RESOURCE_VIEW_DESC srvbuffer_desc;
    ZeroMemory( &srvbuffer_desc, sizeof(srvbuffer_desc) );
    srvbuffer_desc.Format = DXGI_FORMAT_UNKNOWN;
    srvbuffer_desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    srvbuffer_desc.Buffer.ElementWidth = NUM_ELEMENTS;
    hr = g_pd3dDevice->CreateShaderResourceView( g_pBuffer1, &srvbuffer_desc, &g_pBuffer1SRV );
    if( FAILED( hr ) )
        return hr;
#if defined(DEBUG) || defined(PROFILE)
    g_pBuffer1SRV->SetPrivateData( WKPDID_D3DDebugObjectName, sizeof( "Buffer1 SRV" ) - 1, "Buffer1 SRV" );
#endif

    hr = g_pd3dDevice->CreateShaderResourceView( g_pBuffer2, &srvbuffer_desc, &g_pBuffer2SRV );
    if( FAILED( hr ) )
        return hr;
#if defined(DEBUG) || defined(PROFILE)
    g_pBuffer2SRV->SetPrivateData( WKPDID_D3DDebugObjectName, sizeof( "Buffer2 SRV" ) - 1, "Buffer2 SRV" );
#endif

    // Create the Unordered Access View for the Buffers
    // This is used for writing the buffer during the sort and transpose
    D3D11_UNORDERED_ACCESS_VIEW_DESC uavbuffer_desc;
    ZeroMemory( &uavbuffer_desc, sizeof(uavbuffer_desc) );
    uavbuffer_desc.Format = DXGI_FORMAT_UNKNOWN;
    uavbuffer_desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    uavbuffer_desc.Buffer.NumElements = NUM_ELEMENTS;
    hr = g_pd3dDevice->CreateUnorderedAccessView( g_pBuffer1, &uavbuffer_desc, &g_pBuffer1UAV );
    if( FAILED( hr ) )
        return hr;
#if defined(DEBUG) || defined(PROFILE)
     g_pBuffer1UAV->SetPrivateData( WKPDID_D3DDebugObjectName, sizeof( "Buffer1 UAV" ) - 1, "Buffer1 UAV" );
#endif

    hr = g_pd3dDevice->CreateUnorderedAccessView( g_pBuffer2, &uavbuffer_desc, &g_pBuffer2UAV );
    if( FAILED( hr ) )
        return hr;
#if defined(DEBUG) || defined(PROFILE)
    g_pBuffer2UAV->SetPrivateData( WKPDID_D3DDebugObjectName, sizeof( "Buffer2 UAV" ) - 1, "Buffer2 UAV" );
#endif

    // Create the Readback Buffer
    // This is used to read the results back to the CPU
    D3D11_BUFFER_DESC readback_buffer_desc;
    ZeroMemory( &readback_buffer_desc, sizeof(readback_buffer_desc) );
    readback_buffer_desc.ByteWidth = NUM_ELEMENTS * sizeof(UINT);
    readback_buffer_desc.Usage = D3D11_USAGE_STAGING;
    readback_buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    readback_buffer_desc.StructureByteStride = sizeof(UINT);
    hr = g_pd3dDevice->CreateBuffer( &readback_buffer_desc, NULL, &g_pReadBackBuffer );
    if( FAILED( hr ) )
        return hr;
#if defined(DEBUG) || defined(PROFILE)
     g_pReadBackBuffer->SetPrivateData( WKPDID_D3DDebugObjectName, sizeof( "ReadBack" ) - 1, "ReadBack" );
#endif

    return hr;
}

//--------------------------------------------------------------------------------------
// Helper to set the compute shader constants
//--------------------------------------------------------------------------------------
void sorterGPU::SetConstants( UINT iLevel, UINT iLevelMask, UINT iWidth, UINT iHeight )
{
    CB cb = { iLevel, iLevelMask, iWidth, iHeight };
    g_pd3dImmediateContext->UpdateSubresource( g_pCB, 0, NULL, &cb, 0, 0 );
    g_pd3dImmediateContext->CSSetConstantBuffers( 0, 1, &g_pCB );
}

//--------------------------------------------------------------------------------------
// GPU Bitonic Sort
//--------------------------------------------------------------------------------------
void sorterGPU::GPUSort()
{
    // Upload the data
    //g_pd3dImmediateContext->UpdateSubresource( g_pBuffer1, 0, NULL, &data[0], 0, 0 );
	g_pd3dImmediateContext->UpdateSubresource( g_pBuffer1, 0, NULL, data, 0, 0 );

    // Sort the data
    // First sort the rows for the levels <= to the block size
    for( UINT level = 2 ; level <= BITONIC_BLOCK_SIZE ; level = level * 2 )
    {
        SetConstants( level, level, MATRIX_HEIGHT, MATRIX_WIDTH );

        // Sort the row data
        g_pd3dImmediateContext->CSSetUnorderedAccessViews( 0, 1, &g_pBuffer1UAV, NULL );
        g_pd3dImmediateContext->CSSetShader( g_pComputeShaderBitonic, NULL, 0 );
        g_pd3dImmediateContext->Dispatch( NUM_ELEMENTS / BITONIC_BLOCK_SIZE, 1, 1 );
    }
    
    // Then sort the rows and columns for the levels > than the block size
    // Transpose. Sort the Columns. Transpose. Sort the Rows.
    for( UINT level = (BITONIC_BLOCK_SIZE * 2) ; level <= NUM_ELEMENTS ; level = level * 2 )
    {
        SetConstants( (level / BITONIC_BLOCK_SIZE), (level & ~NUM_ELEMENTS) / BITONIC_BLOCK_SIZE, MATRIX_WIDTH, MATRIX_HEIGHT );

        // Transpose the data from buffer 1 into buffer 2
        ID3D11ShaderResourceView* pViewNULL = NULL;
        g_pd3dImmediateContext->CSSetShaderResources( 0, 1, &pViewNULL );
        g_pd3dImmediateContext->CSSetUnorderedAccessViews( 0, 1, &g_pBuffer2UAV, NULL );
        g_pd3dImmediateContext->CSSetShaderResources( 0, 1, &g_pBuffer1SRV );
        g_pd3dImmediateContext->CSSetShader( g_pComputeShaderTranspose, NULL, 0 );
        g_pd3dImmediateContext->Dispatch( MATRIX_WIDTH / TRANSPOSE_BLOCK_SIZE, MATRIX_HEIGHT / TRANSPOSE_BLOCK_SIZE, 1 );

        // Sort the transposed column data
        g_pd3dImmediateContext->CSSetShader( g_pComputeShaderBitonic, NULL, 0 );
        g_pd3dImmediateContext->Dispatch( NUM_ELEMENTS / BITONIC_BLOCK_SIZE, 1, 1 );

        SetConstants( BITONIC_BLOCK_SIZE, level, MATRIX_HEIGHT, MATRIX_WIDTH );

        // Transpose the data from buffer 2 back into buffer 1
        g_pd3dImmediateContext->CSSetShaderResources( 0, 1, &pViewNULL );
        g_pd3dImmediateContext->CSSetUnorderedAccessViews( 0, 1, &g_pBuffer1UAV, NULL );
        g_pd3dImmediateContext->CSSetShaderResources( 0, 1, &g_pBuffer2SRV );
        g_pd3dImmediateContext->CSSetShader( g_pComputeShaderTranspose, NULL, 0 );
        g_pd3dImmediateContext->Dispatch( MATRIX_HEIGHT / TRANSPOSE_BLOCK_SIZE, MATRIX_WIDTH / TRANSPOSE_BLOCK_SIZE, 1 );

        // Sort the row data
        g_pd3dImmediateContext->CSSetShader( g_pComputeShaderBitonic, NULL, 0 );
        g_pd3dImmediateContext->Dispatch( NUM_ELEMENTS / BITONIC_BLOCK_SIZE, 1, 1 );
    }

    // Download the data
    D3D11_MAPPED_SUBRESOURCE MappedResource = {0}; 
    g_pd3dImmediateContext->CopyResource( g_pReadBackBuffer, g_pBuffer1 );
    g_pd3dImmediateContext->Map( g_pReadBackBuffer, 0, D3D11_MAP_READ, 0, &MappedResource );       
    memcpy( result, MappedResource.pData, NUM_ELEMENTS * sizeof(UINT) );
    g_pd3dImmediateContext->Unmap( g_pReadBackBuffer, 0 );
}

