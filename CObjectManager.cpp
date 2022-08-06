#include "CObjectManager.h"
#include <sstream>	// for the multiple ply loader
#include <math.h>	// for rounding

CPlyInfo::CPlyInfo()
{
	this->numberOfVertices = 0;
	this->numberOfElements = 0;
	this->vertexBufferStartIndex = 0;
	this->indexBufferStartIndex = 0;
	this->extent = 1.0f;
};

CPlyInfo::~CPlyInfo() {}


CObjectManager::CObjectManager()
{
	this->m_pVertexBuffer = 0;
	this->m_pIndexBuffer = 0;

	this->m_LastVertexBufferIndex = 0;
	this->m_LastIndexBufferIndex = 0;
}

CObjectManager::~CObjectManager()
{
}


bool CObjectManager::GetPlyRenderingInfoByPlyFileName( std::wstring plyFile, CPlyInfo &plyInfo, std::wstring &error  )
{
	error = L"OK";

	std::map< std::wstring, CPlyInfo >::iterator itObject;
	itObject = this->m_mapPlyObjectInBuffer.find( plyFile );

	// Did we find it?
	if ( itObject == this->m_mapPlyObjectInBuffer.end() )
	{	
		// BUG fix. Need to also look for the double forward slashes for the 
		//	file name... 
		std::wstring plyFileNameDoubleSlash;
		for ( std::wstring::iterator itCurChar = plyFile.begin();
			  itCurChar != plyFile.end(); itCurChar++ )
		{
			wchar_t theChar = *itCurChar;
			// If it's a slash, add another one... 
			if ( theChar == '\\' ) 
			{
				plyFileNameDoubleSlash.push_back( theChar );
			}
			plyFileNameDoubleSlash.push_back( theChar );
		}
		// Now try again, with the double slash version...
		itObject = this->m_mapPlyObjectInBuffer.find( plyFileNameDoubleSlash );

		// Did we find it?
		if ( itObject == this->m_mapPlyObjectInBuffer.end() )
		{	// Nope.
			return false;
		}
		// We found it...with the cursed double slash...
	}

	// Found it...
	plyInfo = itObject->second;		

	return true;
}

template <class T>
T getRand(T lowRange, T highRange)
{
	if ( lowRange > highRange )
	{
		T temp = lowRange;
		lowRange = highRange;
		highRange = temp;
	}
	T delta = highRange - lowRange;
	double theRand = static_cast<double>( rand() ) / static_cast<double>(RAND_MAX);
	T returnVal = static_cast<T>( theRand * delta ) + lowRange;
	return returnVal;
}

CPlyInfo CObjectManager::GetRandomModel( void )
{
	int numModels = static_cast<int>( this->m_mapPlyObjectInBuffer.size() );
	int randModelIndex = getRand< int >( 0, numModels );

	// Go through map until we get to random object
	std::map< std::wstring, CPlyInfo >::iterator itMap = this->m_mapPlyObjectInBuffer.begin();
	int curIndex = 0;
	while ( curIndex != randModelIndex )
	{
		itMap++;
		curIndex++;
	}
	// Now pointint to random model

	CPlyInfo tempPlyInfo = itMap->second;

	return tempPlyInfo;
}

bool CObjectManager::LoadPlyModelTypes( std::vector< std::wstring > vecPlyFiles,
	                                    ID3D10Device* &pd3dDevice,
                                        ID3D10Buffer* &pVertexBuffer, 
										ID3D10Buffer* &pIndexBuffer,
										ID3D10InputLayout* &pVertexLayout,
							            std::wstring &error )
{
	error = L"OK";
	bool allOK = true;

	this->m_pVertexBuffer = pVertexBuffer;
	this->m_pIndexBuffer = pIndexBuffer;
	this->m_pd3dDevice = pd3dDevice;
	this->m_pVertexLayout = pVertexLayout;

	int totalNumberOfVertices = 0;
	int totalNumberOfElements = 0;

	// First, go through the ply files and see how big a vertex buffer we need....
	std::wstringstream ssErrors;
	std::vector< std::wstring >::iterator itModelName;
	for ( itModelName = vecPlyFiles.begin(); itModelName != vecPlyFiles.end(); itModelName++ )
	{
		std::wstring error;
		CPlyFile5nt tempPly;
		if ( !tempPly.ReadPLYFileHeader( *itModelName, error ) )
		{
			ssErrors << error << std::endl;
			allOK = false;
		}
		// Else, it's OK
		totalNumberOfVertices += tempPly.GetNumberOfVerticies();
		totalNumberOfElements += tempPly.GetNumberOfElements();
		// Add it to the map
	}
	
	// Now we want to round up the size to add a bit of padding...
	totalNumberOfVertices = totalNumberOfVertices + (totalNumberOfVertices / 2);
	totalNumberOfElements = totalNumberOfElements + (totalNumberOfElements / 2);

	// Prepare the vertex layout...
    this->m_pd3dDevice->IASetInputLayout( this->m_pVertexLayout );

	// Create a vertex and index buffer big enough to hold ALL the models...
	SimpleVertex* tempVertexArray = new SimpleVertex[ totalNumberOfVertices ];
	memset( tempVertexArray, 0, totalNumberOfVertices * sizeof( SimpleVertex ));

	DWORD* tempIndexArray = new DWORD[ totalNumberOfElements * 3 ];
	memset( tempIndexArray, 0, totalNumberOfElements * 3 * sizeof(DWORD) );


	// Now go through and actually load the ply models...
	for ( itModelName = vecPlyFiles.begin(); itModelName != vecPlyFiles.end(); itModelName++ )
	{
		std::wstring error;
		CPlyInfo tempPlyInfo;
		tempPlyInfo.plyFileName = *itModelName;
		if ( this->m_LoadPlyModelType( tempPlyInfo, tempVertexArray, tempIndexArray, error ) )
		{	// Add this (valid) model to the map
			this->m_mapPlyObjectInBuffer[ tempPlyInfo.plyFileName] = tempPlyInfo;
		}
		else
		{	// Something is wrong
			ssErrors << error << std::endl;
			allOK = false;
		}
	}

	// All the models are in the temporary buffer, so copy them to the vertex and index buffers
	D3D10_BUFFER_DESC bd;
    bd.Usage = D3D10_USAGE_DEFAULT;
	// bd.ByteWidth = sizeof( SimpleVertex ) * 8;
	bd.ByteWidth = sizeof( SimpleVertex ) * totalNumberOfVertices;
    bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;
    bd.MiscFlags = 0;
    D3D10_SUBRESOURCE_DATA InitData;
    InitData.pSysMem = tempVertexArray;
    HRESULT hr = pd3dDevice->CreateBuffer( &bd, &InitData, &pVertexBuffer );
    if( FAILED( hr ) )
	{
		error = L"ERROR: Unable to create vertex buffer.";
		return false;
	}

	bd.Usage = D3D10_USAGE_DEFAULT;
    bd.ByteWidth = sizeof( DWORD ) * totalNumberOfElements * 3;       
    //bd.ByteWidth = sizeof( DWORD ) * 36;        // 36 vertices needed for 12 triangles in a triangle list
    bd.BindFlags = D3D10_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;
    bd.MiscFlags = 0;
    InitData.pSysMem = tempIndexArray;
    hr = pd3dDevice->CreateBuffer( &bd, &InitData, &pIndexBuffer );
    if( FAILED( hr ) )
	{
		error = L"ERROR: Unable to create index buffer";
		return false;
	}



	// If something went wrong, copy the error codes from the stringstream into 
	//	the error string to return
	if ( !allOK )
	{
		error = ssErrors.str();
	}

	// Delete the temporary vertex and index arrays...
	delete [] tempVertexArray;
	delete [] tempIndexArray;

	return allOK;
}

// And the big Kahoona... 
bool CObjectManager::m_LoadPlyModelType( CPlyInfo &tempPlyInfo, 
                                         SimpleVertex* tempVertexArray, 
										 DWORD* tempIndexArray,
										 std::wstring &error )
{
	error = L"OK";

	// Load model, then copy it into the 'current' location in the vertex and index arrays...
	CPlyFile5nt tempPly;
	if ( !tempPly.OpenPLYFile( tempPlyInfo.plyFileName, error ) )
	{
		return false;
	}
	// ply is openned.

	// Calculate the normals for the model...
	tempPly.normalizeTheModelBabyDirectXVersion();

	// Calculate texture coordinates...
	tempPly.GenTextureCoordsSpherical( CPlyFile5nt::POSITIVE_X, CPlyFile5nt::POSITIVE_Y, 
										true, 2.0f, false );

	// **** Indicates where the model is location in the vertex and index buffers
	tempPlyInfo.vertexBufferStartIndex = this->m_LastVertexBufferIndex;
	tempPlyInfo.indexBufferStartIndex = this->m_LastIndexBufferIndex;

	tempPlyInfo.numberOfElements = tempPly.GetNumberOfElements();
	tempPlyInfo.numberOfVertices = tempPly.GetNumberOfVerticies();
	tempPlyInfo.extent = tempPly.getMaxExtent();

	// Copy the 'current' location...
	for ( int index = 0; index != tempPly.GetNumberOfVerticies(); index++ )
	{
		// Get the vertex from the ply model:
		PLYVERTEX tempvert = tempPly.getVertex_at( index );
		//
		int indexOffset = tempPlyInfo.vertexBufferStartIndex + index;
		//
		// Load it into the SimpleVertex type
		tempVertexArray[indexOffset].Pos.x = tempvert.xyz.x;
		tempVertexArray[indexOffset].Pos.y = tempvert.xyz.y;
		tempVertexArray[indexOffset].Pos.z = tempvert.xyz.z;
		tempVertexArray[indexOffset].Pos.w = 1.0f;
		//
		tempVertexArray[indexOffset].Normal.x = tempvert.nx;
		tempVertexArray[indexOffset].Normal.y = tempvert.ny;
		tempVertexArray[indexOffset].Normal.z = tempvert.nz;
		tempVertexArray[indexOffset].Normal.w = 1.0f;		// "Alpha" or transparency	
		//
		tempVertexArray[indexOffset].Texture.x = tempvert.tex0u;
		tempVertexArray[indexOffset].Texture.y = tempvert.tex0v;
	}

	tempPlyInfo.indexBufferStartIndex = this->m_LastIndexBufferIndex;

	// Copy the data into the index buffer...
	for ( int index = 0; index != tempPly.GetNumberOfElements(); index++ )
	{
		PLYELEMENT tempTri = tempPly.getElement_at( index );

		// Calculate the location in the index buffer (stride is 3, not 1)
		int indexBufferIndex = ( index * 3 ) + this->m_LastIndexBufferIndex;

		tempIndexArray[indexBufferIndex]     = tempTri.vertex_index_1;
		tempIndexArray[indexBufferIndex + 1] = tempTri.vertex_index_2;
		tempIndexArray[indexBufferIndex + 2] = tempTri.vertex_index_3;
	}



	// Update the "last" vertex index location...
	this->m_LastVertexBufferIndex += tempPly.GetNumberOfVerticies();
	// Round up to next 16 float offset (just in case): 16 floats = 16 * 4 bytes = 64 bytes
	int multipleOf64Bytes = ( this->m_LastVertexBufferIndex / 64 ) + 1;
	this->m_LastVertexBufferIndex = multipleOf64Bytes * 64; 


	// Update the "last" vertex index location...
	this->m_LastIndexBufferIndex += tempPly.GetNumberOfElements() * 3;
	// Round up to next 16 byte offset (just in case)
	multipleOf64Bytes = ( this->m_LastIndexBufferIndex / 64 ) + 1;
	this->m_LastIndexBufferIndex = multipleOf64Bytes * 64; 

	return true;
}

