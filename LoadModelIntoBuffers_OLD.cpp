#include "global.h"

// **************************************************************************************
// Add a reference to the ply loader...
#include "CPlyFile4nt.h"

bool LoadModelIntoBuffer( std::wstring modelFilename, CPlyInfo &plyInfo, std::wstring &error )
{
	error = L"OK";
	HRESULT hr = S_OK;

	// **************************************************************************************
	// Try to load the ply model from the file (this takes a moment with larger models)
	CPlyFile4nt plyModel;
	if ( !plyModel.OpenPLYFile( modelFilename, error ) )
	{	// We passed the 'error' string, which will be loaded with any errors...
		return false;
	}

	// Get ply information
	plyInfo.numberOfElements = plyModel.GetNumberOfElements();
	plyInfo.numberOfVertices = plyModel.GetNumberOfVerticies();
	plyInfo.extent = plyModel.getMaxExtent();

    // Set the input layout
    g_pd3dDevice->IASetInputLayout( g_pVertexLayout );

	//struct SimpleVertex
	//{
	//	D3DXVECTOR3 Pos;
	//	D3DXVECTOR4 Color;
	//};
    // Create vertex buffer
    //SimpleVertex vertices[] =
    //{
    //    { D3DXVECTOR3( -1.0f, 1.0f, -1.0f ), D3DXVECTOR4( 0.0f, 0.0f, 1.0f, 1.0f ) },
    //    { D3DXVECTOR3( 1.0f, 1.0f, -1.0f ), D3DXVECTOR4( 0.0f, 1.0f, 0.0f, 1.0f ) },
    //    { D3DXVECTOR3( 1.0f, 1.0f, 1.0f ), D3DXVECTOR4( 0.0f, 1.0f, 1.0f, 1.0f ) },
    //    { D3DXVECTOR3( -1.0f, 1.0f, 1.0f ), D3DXVECTOR4( 1.0f, 0.0f, 0.0f, 1.0f ) },
    //    { D3DXVECTOR3( -1.0f, -1.0f, -1.0f ), D3DXVECTOR4( 1.0f, 0.0f, 1.0f, 1.0f ) },
    //    { D3DXVECTOR3( 1.0f, -1.0f, -1.0f ), D3DXVECTOR4( 1.0f, 1.0f, 0.0f, 1.0f ) },
    //    { D3DXVECTOR3( 1.0f, -1.0f, 1.0f ), D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f ) },
    //    { D3DXVECTOR3( -1.0f, -1.0f, 1.0f ), D3DXVECTOR4( 0.0f, 0.0f, 0.0f, 1.0f ) },
    //};

	// Allocate enough space (multiply by 2 to give some extra space if memory is not "aligned")
	// Yes, it's a hack, but we have lots of memory. 
	int numberOfVerticesInBuffer = plyModel.GetNumberOfVerticies() * 2;
	SimpleVertex* vertices = new SimpleVertex[ numberOfVerticesInBuffer ];
	memset( vertices, 0, numberOfVerticesInBuffer * sizeof( SimpleVertex ));

	// ********************************************************************
	// Calculate the normals for the model...
	plyModel.normalizeTheModelBabyDirectXVersion();
	// ********************************************************************

	// Copy the data from the PLY file into this vertex buffer array...
	for ( int index = 0; index != plyModel.GetNumberOfVerticies(); index++ )
	{
		// Get the vertex from the ply model:
		PLYVERTEX tempvert = plyModel.getVertex_at( index );
		// Load it into the SimpleVertex type
		vertices[index].Pos.x = tempvert.xyz.x;
		vertices[index].Pos.y = tempvert.xyz.y;
		vertices[index].Pos.z = tempvert.xyz.z;
		vertices[index].Pos.w = 1.0f;
		// There are no colours in this PLY file, so the colours are all 0,0,0
		//	... which is black, so we can't see it. Let's change it to something else
		vertices[index].Normal.x = tempvert.nx;
		vertices[index].Normal.y = tempvert.ny;
		vertices[index].Normal.z = tempvert.nz;
		vertices[index].Normal.w = 1.0f;		// "Alpha" or transparency	
	}

    D3D10_BUFFER_DESC bd;
    bd.Usage = D3D10_USAGE_DEFAULT;
	// bd.ByteWidth = sizeof( SimpleVertex ) * 8;
	bd.ByteWidth = sizeof( SimpleVertex ) * numberOfVerticesInBuffer;
    bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;
    bd.MiscFlags = 0;
    D3D10_SUBRESOURCE_DATA InitData;
    InitData.pSysMem = vertices;
    hr = g_pd3dDevice->CreateBuffer( &bd, &InitData, &g_pVertexBuffer );
    if( FAILED( hr ) )
	{
		error = L"ERROR: Unable to create vertex buffer.";
		return false;
	}

    // Set vertex buffer
    //UINT stride = sizeof( SimpleVertex );
    //UINT offset = 0;
    //g_pd3dDevice->IASetVertexBuffers( 0, 1, &g_pVertexBuffer, &stride, &offset );

    // Create index buffer
    //DWORD indices[] =
    //{
    //    3,1,0,
    //    2,1,3,

    //    0,5,4,
    //    1,5,0,

    //    3,4,7,
    //    0,4,3,

    //    1,6,5,
    //    2,6,1,

    //    2,7,6,
    //    3,7,2,

    //    6,4,5,
    //    7,4,6,
    //};
	int numberOfIndicesInBuffer = plyModel.GetNumberOfElements() * 2;
	// The "* 3" is because there are three indices per triangle...
	DWORD* indices = new DWORD[ numberOfIndicesInBuffer * 3 ];
	// Clear the buffer.
	memset( indices, 0, numberOfIndicesInBuffer * 3 * sizeof(DWORD) );

	// Copy the data into the index buffer...
	for ( int index = 0; index != plyModel.GetNumberOfElements(); index++ )
	{
		PLYELEMENT tempTri = plyModel.getElement_at( index );
		// Calculate the location in the index buffer (stride is 3, not 1)
		int indexBufferIndex = index * 3;
		indices[indexBufferIndex]     = tempTri.vertex_index_1;
		indices[indexBufferIndex + 1] = tempTri.vertex_index_2;
		indices[indexBufferIndex + 2] = tempTri.vertex_index_3;
	}

    bd.Usage = D3D10_USAGE_DEFAULT;
    bd.ByteWidth = sizeof( DWORD ) * numberOfIndicesInBuffer * 3;       
    //bd.ByteWidth = sizeof( DWORD ) * 36;        // 36 vertices needed for 12 triangles in a triangle list
    bd.BindFlags = D3D10_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;
    bd.MiscFlags = 0;
    InitData.pSysMem = indices;
    hr = g_pd3dDevice->CreateBuffer( &bd, &InitData, &g_pIndexBuffer );
    if( FAILED( hr ) )
	{
		error = L"ERROR: Unable to create index buffer";
		return false;
	}

	//// Set index buffer
 //   g_pd3dDevice->IASetIndexBuffer( g_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0 );



	return true;
}


// *************************************************************************************
// *************************************************************************************
// Here's the old function for reference. 
// Note that you can't call it as its name has changed...
// (You can comment it out if you'd like)
bool LoadModelIntoBuffer_OLD_DRAWS_A_CUBE( std::wstring modelFilename, std::wstring &error )
{
	error = L"OK";
	HRESULT hr = S_OK;


    // Set the input layout
    g_pd3dDevice->IASetInputLayout( g_pVertexLayout );

    // Create vertex buffer
    SimpleVertex vertices[] =
    {
        { D3DXVECTOR3( -1.0f, 1.0f, -1.0f ), D3DXVECTOR4( 0.0f, 0.0f, 1.0f, 1.0f ) },
        { D3DXVECTOR3( 1.0f, 1.0f, -1.0f ), D3DXVECTOR4( 0.0f, 1.0f, 0.0f, 1.0f ) },
        { D3DXVECTOR3( 1.0f, 1.0f, 1.0f ), D3DXVECTOR4( 0.0f, 1.0f, 1.0f, 1.0f ) },
        { D3DXVECTOR3( -1.0f, 1.0f, 1.0f ), D3DXVECTOR4( 1.0f, 0.0f, 0.0f, 1.0f ) },
        { D3DXVECTOR3( -1.0f, -1.0f, -1.0f ), D3DXVECTOR4( 1.0f, 0.0f, 1.0f, 1.0f ) },
        { D3DXVECTOR3( 1.0f, -1.0f, -1.0f ), D3DXVECTOR4( 1.0f, 1.0f, 0.0f, 1.0f ) },
        { D3DXVECTOR3( 1.0f, -1.0f, 1.0f ), D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f ) },
        { D3DXVECTOR3( -1.0f, -1.0f, 1.0f ), D3DXVECTOR4( 0.0f, 0.0f, 0.0f, 1.0f ) },
    };
    D3D10_BUFFER_DESC bd;
    bd.Usage = D3D10_USAGE_DEFAULT;
    bd.ByteWidth = sizeof( SimpleVertex ) * 8;
    bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;
    bd.MiscFlags = 0;
    D3D10_SUBRESOURCE_DATA InitData;
    InitData.pSysMem = vertices;
    hr = g_pd3dDevice->CreateBuffer( &bd, &InitData, &g_pVertexBuffer );
    if( FAILED( hr ) )
	{
		error = L"ERROR: Unable to create vertex buffer.";
		return false;
	}

    // Set vertex buffer
    UINT stride = sizeof( SimpleVertex );
    UINT offset = 0;
    g_pd3dDevice->IASetVertexBuffers( 0, 1, &g_pVertexBuffer, &stride, &offset );

    // Create index buffer
    DWORD indices[] =
    {
        3,1,0,
        2,1,3,

        0,5,4,
        1,5,0,

        3,4,7,
        0,4,3,

        1,6,5,
        2,6,1,

        2,7,6,
        3,7,2,

        6,4,5,
        7,4,6,
    };
    bd.Usage = D3D10_USAGE_DEFAULT;
    bd.ByteWidth = sizeof( DWORD ) * 36;        // 36 vertices needed for 12 triangles in a triangle list
    bd.BindFlags = D3D10_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;
    bd.MiscFlags = 0;
    InitData.pSysMem = indices;
    hr = g_pd3dDevice->CreateBuffer( &bd, &InitData, &g_pIndexBuffer );
    if( FAILED( hr ) )
	{
		error = L"ERROR: Unable to create index buffer";
		return false;
	}

	// Set index buffer
    g_pd3dDevice->IASetIndexBuffer( g_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0 );



	return true;
}