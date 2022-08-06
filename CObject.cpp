#include "CObject.h"

CObject::CObject()
{

	this->Scale = 1.0f;
	//this->red = this->green = this->blue = 1.0f;		// White
	//
	this->numTriangles = 0;
	this->numVertices = 0;
	this->maxExtent = 1.0f;
	// 
	//this->RotSpeedX = this->RotSpeedY = this->RotSpeedZ = 0.0f;

	// Clear the matrix for this object
	D3DXMatrixIdentity( &(this->myWorldMatrix) );

	// Assume that it's being drawn relative to the origin
	this->bIsRootNodeSetToOrigin = true;

	std::string name;

}

CObject::~CObject()
{

}