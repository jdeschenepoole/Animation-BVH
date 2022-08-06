#ifndef _CObjectManager_HG_
#define _CObjectManager_HG_

#include "CPlyFile5nt.h"
#include <vector>
#include <map>
#include <d3d10.h>
#include <d3dx10.h>

#include "CObject.h"

class CPlyInfo
{
public:
	CPlyInfo();
	~CPlyInfo();
	int numberOfVertices;
	int numberOfElements;
	int vertexBufferStartIndex;
	int indexBufferStartIndex;
	float extent;
	std::wstring plyFileName;
};

struct SimpleVertex
{
    D3DXVECTOR4 Pos;
    D3DXVECTOR4 Normal;
	D3DXVECTOR2 Texture;		// Added for texturing...
};

class CObjectManager
{
public:
	CObjectManager();
	~CObjectManager();

	// This takes a vector of ply file names, then loads them into an single
	//	vertex and index buffer. 
	bool LoadPlyModelTypes( std::vector< std::wstring > vecPlyFiles, 
		                    ID3D10Device* &pd3dDevice,
	                        ID3D10Buffer* &pVertexBuffer,
							ID3D10Buffer* &pIndexBuffer,
							ID3D10InputLayout* &pVertexLayout,
							std::wstring &error );

	bool GetPlyRenderingInfoByPlyFileName( std::wstring plyFile, CPlyInfo &plyInfo, std::wstring &error );

	CPlyInfo GetRandomModel( void );

private:
	// Ordered by ply file name. 
	std::map< std::wstring, CPlyInfo > m_mapPlyObjectInBuffer;

	// Used internally to load each ply...
	bool m_LoadPlyModelType( CPlyInfo &tempPlyInfo, 
                             SimpleVertex* tempVertexArray, 
							 DWORD* tempIndexArray,
							 std::wstring &error );
	ID3D10Buffer*		m_pVertexBuffer;
	ID3D10Buffer*       m_pIndexBuffer;
	ID3D10Device*		m_pd3dDevice;
	ID3D10InputLayout*  m_pVertexLayout;

	int m_LastVertexBufferIndex;
	int m_LastIndexBufferIndex;
};


#endif