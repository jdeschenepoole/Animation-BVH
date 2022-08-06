#ifndef _CObject_HG_
#define _CObject_HG_

#include <string>
#include "VariousTypes.h"
#include <d3dx10.h>	
#include <vector>

class CObject
{
public:
	CObject();
	virtual ~CObject();

	CVector3f preRotationXYZ;
	CVector3f translationXYZ;
	CVector3f postRotationXYZ;
	
	CVector3f preRotationSpeedXYZ;
	CVector3f postRotationSpeedXYZ;

	float Scale;
	//float red, green, blue;
	MaterialInfo material;
	// 
	int numTriangles;
	int numVertices;
	float maxExtent;
	std::wstring plyFileName;
	std::wstring name;

	// This will store the world matrix of the object
	::D3DXMATRIX myWorldMatrix;
	// If this is true, then the object is 
	//	based on the world origin (as opposed to
	//	being relative to a 'parent' object)
	bool bIsRootNodeSetToOrigin;	

	// This will store all the child objects...
	std::vector< CObject > vecChildObjects;
};

#endif