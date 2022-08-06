// Written by Michael Feeney, Fanshawe College, 2010
// mfeeney@fanshawec.on.ca
// It may be distributed under the terms of the General Public License:
// http://www.fsf.org/licenses/gpl.html
// Use this code at your own risk. It is indented only as a learning aid.
//
#include "CPlyFile5nt.h"

#include <fstream>
// *********************
// **** If you are using the DirectX methods, add these... ****
//#include <d3d9.h>
//#include <D3dx9math.h>
#include <d3d10.h>
#include <d3dx10.h>

#include <sstream>
#include <math.h>
#include <algorithm>

// Written by Michael Feeney, Fanshawe College, 2009
// mfeeney@fanshawec.ca
// It may be distributed under the terms of the General Public License:
// http://www.fsf.org/licenses/gpl.html
// Use this code at your own risk. It is indented only as a learning aid.

// Very simply ply reader that also calculates the normal for the model.
//
// See Greg Turk's paper and details of the bunny here:
// http://www.cc.gatech.edu/~turk/bunny/bunny.html

const float PI = 3.141592653589793238462643383279502884197f;	// Yes, yes, I *know* it's too big... whatever.

CPlyFile5nt::CPlyFile5nt() 
{
	this->m_minX = this->m_maxX = this->m_deltaX = 0.0f;
	this->m_minY = this->m_maxY = this->m_deltaY = 0.0f;
	this->m_minZ = this->m_maxZ = this->m_deltaZ = 0.0f;
	this->m_centreX = this->m_centreY = this->m_centreZ = 0.0f;
	//
	this->m_ID = statNextID++;
	//
	this->m_numberOfVertices = 0;
	this->m_numberOfElements = 0;
}


//static 
unsigned int CPlyFile5nt::statNextID = 1;

CPlyFile5nt::~CPlyFile5nt()
{
}

CPlyFile5nt::CPlyFile5nt( const CPlyFile5nt &rhs )	// Copy constructor
{
	this->m_verticies = rhs.m_verticies;
	this->m_elements = rhs.m_elements;

	this->m_minX = rhs.m_minX;
	this->m_minY = rhs.m_minY;
	this->m_minZ = rhs.m_minZ;

	this->m_maxX = rhs.m_maxX;
	this->m_maxY = rhs.m_maxY;
	this->m_maxZ = rhs.m_maxZ;

	this->m_deltaX = rhs.m_deltaX;
	this->m_deltaY = rhs.m_deltaY;
	this->m_deltaZ = rhs.m_deltaZ;

	this->m_centreX = rhs.m_centreX;
	this->m_centreY = rhs.m_centreY;
	this->m_centreZ = rhs.m_centreZ;

	this->m_maxExtent = 0.0f;

	this->m_totalProperties = this->m_totalProperties;
	this->m_x_propertyIndex = this->m_x_propertyIndex;
	this->m_y_propertyIndex = this->m_y_propertyIndex;
	this->m_z_propertyIndex = this->m_z_propertyIndex;
	this->m_normx_propertyIndex = this->m_normx_propertyIndex;
	this->m_normy_propertyIndex = this->m_normy_propertyIndex;
	this->m_normz_propertyIndex = this->m_normz_propertyIndex;
	this->m_red_propertyIndex = this->m_red_propertyIndex;
	this->m_green_propertyIndex = this->m_green_propertyIndex;
	this->m_blue_propertyIndex = this->m_blue_propertyIndex;
}

CPlyFile5nt& CPlyFile5nt::operator=( const CPlyFile5nt &rhs )
{
	// Check for self-assignment *IMPORTANT*
	if ( this == &rhs )	return *this;
	// Else...copy values
	this->m_verticies = rhs.m_verticies;
	this->m_elements = rhs.m_elements;

	this->m_minX = rhs.m_minX;
	this->m_minY = rhs.m_minY;
	this->m_minZ = rhs.m_minZ;

	this->m_maxX = rhs.m_maxX;
	this->m_maxY = rhs.m_maxY;
	this->m_maxZ = rhs.m_maxZ;

	this->m_deltaX = rhs.m_deltaX;
	this->m_deltaY = rhs.m_deltaY;
	this->m_deltaZ = rhs.m_deltaZ;

	this->m_centreX = rhs.m_centreX;
	this->m_centreY = rhs.m_centreY;
	this->m_centreZ = rhs.m_centreZ;

	this->m_totalProperties = this->m_totalProperties;
	this->m_x_propertyIndex = this->m_x_propertyIndex;
	this->m_y_propertyIndex = this->m_y_propertyIndex;
	this->m_z_propertyIndex = this->m_z_propertyIndex;
	this->m_normx_propertyIndex = this->m_normx_propertyIndex;
	this->m_normy_propertyIndex = this->m_normy_propertyIndex;
	this->m_normz_propertyIndex = this->m_normz_propertyIndex;
	this->m_red_propertyIndex = this->m_red_propertyIndex;
	this->m_green_propertyIndex = this->m_green_propertyIndex;
	this->m_blue_propertyIndex = this->m_blue_propertyIndex;

	return *this;
}

unsigned int CPlyFile5nt::GetID( void ) 
{
	return this->m_ID;
}

bool CPlyFile5nt::ReadPLYFileHeader(std::wstring fileName, std::wstring &error)
{
		error = L"OK";

	std::wifstream thePlyFile(fileName.c_str());
	if (!thePlyFile.is_open())
	{
		error = L"Can't open the file. Sorry it didn't work out.";
		return false;
	}

	std::wstring tempString;
	this->m_numberOfVertices = 0;
	this->m_numberOfElements = 0;

	try
	{
		bool bItsABunnyFile = true;
		// ply
		thePlyFile >> tempString;	if ( tempString != L"ply")		{ bItsABunnyFile = false; error = L"No a ply file."; throw; }
		// format ascii 1.0
		thePlyFile >> tempString;	if ( tempString != L"format")	{ bItsABunnyFile = false; throw; }
		thePlyFile >> tempString;	if ( tempString != L"ascii")	{ bItsABunnyFile = false; error = L"Must be an ASCII ply file."; throw; }
		thePlyFile >> tempString;	if ( tempString != L"1.0")		{ bItsABunnyFile = false; throw; }

		// Find the number of verticies...
		// Item after "vertex" is number of verticies
		while ( thePlyFile >> tempString )
		{
			if ( tempString == L"vertex" )	break;
		}			
		thePlyFile >> this->m_numberOfVertices;	// 453

		// Read the properties and note the index locations of them...
		int currentIndex = 0;
		while ( true )
		{
			thePlyFile >> tempString; 
			if ( tempString == L"element" ) { break; /* on to next part of file */ }
			// property float x
			if ( tempString == L"property") 
			{
				thePlyFile >> tempString; 
				if ( ( tempString == L"float") | ( tempString == L"uchar") )	
				{	// Figure out which index to set
					thePlyFile >> tempString;
					this->m_setIndexBasedOnPropertyName( currentIndex, tempString );
				}
				else 
				{ 
					error = L"expected property type missing"; throw; 
				} // if ( tempString == L"float")	
			}
			else
			{ 
				error = L"expected property"; throw; 
			} // if ( tempString == L"property") 
			currentIndex++;
		}
		this->m_totalProperties = currentIndex;
		// 
		thePlyFile >> tempString;	if ( tempString != L"face")	{ bItsABunnyFile = false; error = L"expected element face."; throw; }
		thePlyFile >> this->m_numberOfElements;	

		while ( thePlyFile >> tempString )
		{ 
			if ( tempString == L"end_header" )	break;
		}
	}
	catch (...)
	{
		error = L"Something went wrong while reading the file.";
		return false;
	}

	return true;
}

bool CPlyFile5nt::OpenPLYFile(std::wstring fileName, std::wstring &error)
{
	error = L"OK";

	std::wifstream thePlyFile(fileName.c_str());
	if (!thePlyFile.is_open())
	{
		error = L"Can't open the file. Sorry it didn't work out.";
		return false;
	}

	std::wstring tempString;
	this->m_numberOfVertices = 0;
	this->m_numberOfElements = 0;

	try
	{
		bool bItsABunnyFile = true;
		// ply
		thePlyFile >> tempString;	if ( tempString != L"ply")		{ bItsABunnyFile = false; error = L"No a ply file."; throw; }
		// format ascii 1.0
		thePlyFile >> tempString;	if ( tempString != L"format")	{ bItsABunnyFile = false; throw; }
		thePlyFile >> tempString;	if ( tempString != L"ascii")	{ bItsABunnyFile = false; error = L"Must be an ASCII ply file."; throw; }
		thePlyFile >> tempString;	if ( tempString != L"1.0")		{ bItsABunnyFile = false; throw; }

		// Find the number of verticies...
		// Item after "vertex" is number of verticies
		while ( thePlyFile >> tempString )
		{
			if ( tempString == L"vertex" )	break;
		}			
		thePlyFile >> this->m_numberOfVertices;	// 453

		// Read the properties and note the index locations of them...
		int currentIndex = 0;
		while ( true )
		{
			thePlyFile >> tempString; 
			if ( tempString == L"element" ) { break; /* on to next part of file */ }
			// property float x
			if ( tempString == L"property") 
			{
				thePlyFile >> tempString; 
				if ( ( tempString == L"float") | ( tempString == L"uchar") )	
				{	// Figure out which index to set
					thePlyFile >> tempString;
					this->m_setIndexBasedOnPropertyName( currentIndex, tempString );
				}
				else 
				{ 
					error = L"expected property type missing"; throw; 
				} // if ( tempString == L"float")	
			}
			else
			{ 
				error = L"expected property"; throw; 
			} // if ( tempString == L"property") 
			currentIndex++;
		}
		this->m_totalProperties = currentIndex;
		// 
		thePlyFile >> tempString;	if ( tempString != L"face")	{ bItsABunnyFile = false; error = L"expected element face."; throw; }
		thePlyFile >> this->m_numberOfElements;	

		while ( thePlyFile >> tempString )
		{ 
			if ( tempString == L"end_header" )	break;
		}

		// Now the real work begins...
		//... a bunch of vertices...
		//-0.0312216 0.126304 0.00514924 0.850855 0.5 
		for (int vertexCount = 0; vertexCount != this->m_numberOfVertices; vertexCount++)
		{
			PLYVERTEX tempVertex;
			// Place the data into the array...
			// Read through the properties and place them into the appropriate part of the vertex...
			for (int curPropIndex = 0; curPropIndex != this->m_totalProperties; curPropIndex++)
			{
				std::wstring tempString;
				thePlyFile >> tempString;
				// Load this item based on index
				this->m_placeItemInAppropriateVertexLocation( tempString, curPropIndex, tempVertex );
			}
			//thePlyFile >> tempVertex.x;
			//thePlyFile >> tempVertex.y;
			//thePlyFile >> tempVertex.z;
			//thePlyFile >> tempVertex.confidence;
			//thePlyFile >> tempVertex.intensity;
			// Check bounding box limits...
			if ( tempVertex.xyz.x < this->m_minX )	this->m_minX = tempVertex.xyz.x;
			if ( tempVertex.xyz.x > this->m_maxX )	this->m_maxX = tempVertex.xyz.x;
			if ( tempVertex.xyz.y < this->m_minY )	this->m_minY = tempVertex.xyz.y;
			if ( tempVertex.xyz.y > this->m_maxY )	this->m_maxY = tempVertex.xyz.y;
			if ( tempVertex.xyz.z < this->m_minZ )	this->m_minZ = tempVertex.xyz.z;
			if ( tempVertex.xyz.z > this->m_maxZ )	this->m_maxZ = tempVertex.xyz.z;
			this->m_deltaX = this->m_maxX - this->m_minX;
			this->m_deltaY = this->m_maxY - this->m_minY;
			this->m_deltaZ = this->m_maxZ - this->m_minZ;
			// 
			if ( this->m_deltaX > this->m_maxExtent )	this->m_maxExtent = this->m_deltaX;
			if ( this->m_deltaY > this->m_maxExtent )	this->m_maxExtent = this->m_deltaY;
			if ( this->m_deltaZ > this->m_maxExtent )	this->m_maxExtent = this->m_deltaZ;

			this->m_verticies.push_back(tempVertex);
		}
		// Now read the elements (the triangles...)
		//... a bunch of elements that refer to the verticies
		//3 435 423 430 
		for (int elementCount = 0; elementCount != this->m_numberOfElements; elementCount++)
		{
			PLYELEMENT tempElement;
			thePlyFile >> tempString;	// 3 
			thePlyFile >> tempElement.vertex_index_1;
			thePlyFile >> tempElement.vertex_index_2;
			thePlyFile >> tempElement.vertex_index_3;
			this->m_elements.push_back( tempElement );
		}
	}
	catch (...)
	{
		error = L"Something went wrong while reading the file.";
		return false;
	}

	
	return true;
}



void NormalizeVector(float &x, float &y, float &z)
{
	// Convert to double so there is no loss...
	double x2 = static_cast<double>(x) * static_cast<double>(x);
	double y2 = static_cast<double>(y) * static_cast<double>(y);
	double z2 = static_cast<double>(z) * static_cast<double>(z);
	double length = x2 + y2 + z2;
	if ( length == 0 )
	{
		x = y = z = 0.0f;
	} 
	else
	{	// Multiply by reciprical as multiplies are faster than divides (usually)
		double reciprical = 1 / sqrt( length );
	x = static_cast<float>( static_cast<double>(x) * reciprical );
	y = static_cast<float>( static_cast<double>(y) * reciprical  );
	z = static_cast<float>( static_cast<double>(z) * reciprical  );
	}
}


// Takes cross of vec0 (x0, y0, z0) and vec1 (x1, y1, z1) and 
//	returns difference (xSub, ySub, zSub)
void VectorSubtract(float &x0, float &y0, float &z0, 
					float &x1, float &y1, float &z1,
					float &xSub, float &ySub, float &zSub)
{
	xSub = x0 - x1;
	ySub = y0 - y1;
	zSub = z0 - z1;
}

// Takes cross of vec0 (x0, y0, z0) and vec1 (x1, y1, z1) and 
//	returns cross (xCross, yCross, zCross)
void CrossProduct(float &x0, float &y0, float &z0, 
				  float &x1, float &y1, float &z1,
				  float &xCross, float &yCross, float &zCross)
{
    /* find cross-product between these vectors */
    xCross = y0 * z1 - z0 * y1;
    yCross = z0 * x1 - x0 * z1;
    zCross = x0 * y1 - y0 * x1;
}

// ******************************************************************************************
// ******************************************************************************************
// ******************************************************************************************
//                         _ _        _____ _        __  __        _     _ ___      _         
// _ _  ___ _ _ _ __  __ _| (_)______|_   _| |_  ___|  \/  |___ __| |___| | _ )__ _| |__ _  _ 
//| ' \/ _ \ '_| '  \/ _` | | |_ / -_) | | | ' \/ -_) |\/| / _ | _` / -_) | _ | _` | '_ \ || |
//|_||_\___/_| |_|_|_\__,_|_|_/__\___| |_| |_||_\___|_|  |_\___|__,_\___|_|___|__,_|_.__/\_, |
//                                                                                       |__/ 
void CPlyFile5nt::normalizeTheModelBaby(void)
{
	// The first part of this is the same as the 'rock-n-roll' versions,
	//	but instead of just overwriting the vertices at each face, the
	//	normalized normal is ADDED to the existing normal at that face.
	// Once we are all done, we go through each vertex an normalize them.
	// What this effectively does is get the AVERAGE of all the normals 
	//	of the faces around each vertes.
	// It's only slightly more complicated, and is more 'proper.'
	// LONG WAY version
	std::vector<PLYELEMENT>::iterator itVecFace;
	for ( itVecFace = this->m_elements.begin(); itVecFace != this->m_elements.end(); itVecFace++)
	{	// A simple, local struct to help...
		//struct SexyVector
		//{
		//	SexyVector() : x(0.0f), y(0.0f), z(0.0f) {};
		//	float x, y, z;
		//};
		// Get the three corners (verticies) of the triangle
		//SexyVector vectorA, vectorB, vectorC;
		CVector3f vectorA, vectorB, vectorC;
		vectorA.x = this->m_verticies.at( itVecFace->vertex_index_1 ).xyz.x;
		vectorA.y = this->m_verticies.at( itVecFace->vertex_index_1 ).xyz.y;
		vectorA.z = this->m_verticies.at( itVecFace->vertex_index_1 ).xyz.z;
		vectorB.x = this->m_verticies.at( itVecFace->vertex_index_2 ).xyz.x;
		vectorB.y = this->m_verticies.at( itVecFace->vertex_index_2 ).xyz.y;
		vectorB.z = this->m_verticies.at( itVecFace->vertex_index_2 ).xyz.z;
		vectorC.x = this->m_verticies.at( itVecFace->vertex_index_3 ).xyz.x;
		vectorC.y = this->m_verticies.at( itVecFace->vertex_index_3 ).xyz.y;
		vectorC.z = this->m_verticies.at( itVecFace->vertex_index_3 ).xyz.z;
		// calculate the vectors for the cross...
		//SexyVector vecAB;// = vecB - vecA
		CVector3f vecAB;
		//::VectorSubtract( vectorB.x, vectorB.y, vectorB.z,
		//	              vectorA.x, vectorA.y, vectorA.z,
		//				  vecAB.x, vecAB.y, vecAB.z );		
		vecAB = vectorB - vectorA;
		
		//SexyVector vecBC;// = vecC - vecB
		CVector3f vecBC;
		//::VectorSubtract( vectorC.x, vectorC.y, vectorC.z,
		//	              vectorB.x, vectorB.y, vectorB.z,
		//				  vecBC.x, vecBC.y, vecBC.z );
		vecBC = vectorC - vectorB;

		//SexyVector normal;
		CVector3f normal;
		//::CrossProduct( vecAB.x, vecAB.y, vecAB.z, 
		//	            vecBC.x, vecBC.y, vecBC.z, 
		//				normal.x, normal.y, normal.z );
		normal.CrossProduct( vecAB, vecBC, normal );

		//::NormalizeVector( normal.x, normal.y, normal.z );
		normal.Normalize();

		// Load the normals onto the verticies
		// NOTE: We are ADDING instead of OVERWRITING...
		this->m_verticies.at( itVecFace->vertex_index_1 ).nx += normal.x;  //****
		this->m_verticies.at( itVecFace->vertex_index_1 ).ny += normal.y;  //****
		this->m_verticies.at( itVecFace->vertex_index_1 ).nz += normal.z;  //****
		this->m_verticies.at( itVecFace->vertex_index_2 ).nx += normal.x;  //****
		this->m_verticies.at( itVecFace->vertex_index_2 ).ny += normal.y;  //****
		this->m_verticies.at( itVecFace->vertex_index_2 ).nz += normal.z;  //****
		this->m_verticies.at( itVecFace->vertex_index_3 ).nx += normal.x;  //****
		this->m_verticies.at( itVecFace->vertex_index_3 ).ny += normal.y;  //****
		this->m_verticies.at( itVecFace->vertex_index_3 ).nz += normal.z;  //****
	}
	// Now go through all the vertices and normalize (average) them...
	std::vector<PLYVERTEX>::iterator itVert;
	for ( itVert = this->m_verticies.begin(); itVert != this->m_verticies.end(); itVert++)
	{	
		::NormalizeVector( itVert->nx, itVert->ny, itVert->nz );
	}
}

//                         _ _        _____ _        __  __        _     _ ___      _         ___  _            _  __  ____   __            _          
// _ _  ___ _ _ _ __  __ _| (_)______|_   _| |_  ___|  \/  |___ __| |___| | _ )__ _| |__ _  _|   \(_)_ _ ___ __| |_\ \/ /\ \ / /___ _ _ ___(_)___ _ _  
//| ' \/ _ \ '_| '  \/ _` | | |_ / -_) | | | ' \/ -_) |\/| / _ | _` / -_) | _ | _` | '_ \ || | |) | | '_/ -_) _|  _|>  <  \ V // -_) '_(_-<| / _ \ ' \ 
//|_||_\___/_| |_|_|_\__,_|_|_/__\___| |_| |_||_\___|_|  |_\___|__,_\___|_|___|__,_|_.__/\_, |___/|_|_| \___\__|\__/_/\_\  \_/ \___|_| /__/|_\___/_||_|
//                                                                                       |__/                                                          
void CPlyFile5nt::normalizeTheModelBabyDirectXVersion(void)
{
	// The first part of this is the same as the 'rock-n-roll' versions,
	//	but instead of just overwriting the vertices at each face, the
	//	normalized normal is ADDED to the existing normal at that face.
	// Once we are all done, we go through each vertex an normalize them.
	// What this effectively does is get the AVERAGE of all the normals 
	//	of the faces around each vertes.
	// It's only slightly more complicated, and is more 'proper.'
	// DIRECTX Version
	std::vector<PLYELEMENT>::iterator itVecFace;
	for ( itVecFace = this->m_elements.begin(); itVecFace != this->m_elements.end(); itVecFace++)
	{	
		// Get the three corners (verticies) of the triangle
		PLYVERTEX vertexA = this->m_verticies.at( itVecFace->vertex_index_1 );
		PLYVERTEX vertexB = this->m_verticies.at( itVecFace->vertex_index_2 );
		PLYVERTEX vertexC = this->m_verticies.at( itVecFace->vertex_index_3 );
		// Now calculate the vectors...
		D3DXVECTOR3 vectorA( vertexA.xyz.x, vertexA.xyz.y, vertexA.xyz.z );
		D3DXVECTOR3 vectorB( vertexB.xyz.x, vertexB.xyz.y, vertexB.xyz.z );
		D3DXVECTOR3 vectorC( vertexC.xyz.x, vertexC.xyz.y, vertexC.xyz.z );
		// calculate the vectors for the cross...
		D3DXVECTOR3 vecAB = vectorB - vectorA;
		D3DXVECTOR3 vecBC = vectorC - vectorB;
		D3DXVECTOR3 normal;
		D3DXVec3Cross( &normal, &vecAB, &vecBC );
		D3DXVec3Normalize( &normal, &normal );
		// Load the normals onto the verticies
		// NOTE: We are ADDING instead of OVERWRITING...
		this->m_verticies.at( itVecFace->vertex_index_1 ).nx += normal.x;  //****
		this->m_verticies.at( itVecFace->vertex_index_1 ).ny += normal.y;  //****
		this->m_verticies.at( itVecFace->vertex_index_1 ).nz += normal.z;  //****
		this->m_verticies.at( itVecFace->vertex_index_2 ).nx += normal.x;  //****
		this->m_verticies.at( itVecFace->vertex_index_2 ).ny += normal.y;  //****
		this->m_verticies.at( itVecFace->vertex_index_2 ).nz += normal.z;  //****
		this->m_verticies.at( itVecFace->vertex_index_3 ).nx += normal.x;  //****
		this->m_verticies.at( itVecFace->vertex_index_3 ).ny += normal.y;  //****
		this->m_verticies.at( itVecFace->vertex_index_3 ).nz += normal.z;  //****
	}
	// Now go through all the vertices and normalize (average) them...
	std::vector<PLYVERTEX>::iterator itVert;
	for ( itVert = this->m_verticies.begin(); itVert != this->m_verticies.end(); itVert++)
	{	
		D3DXVECTOR3 normal( itVert->nx, itVert->ny, itVert->nz );
		D3DXVec3Normalize( &normal, &normal );
		itVert->nx = normal.x;
		itVert->ny = normal.y;
		itVert->nz = normal.z;
	}
}
//                         _ _        _____ _        __  __        _     _ ___      _             ___        _                ___     _ _ 
// _ _  ___ _ _ _ __  __ _| (_)______|_   _| |_  ___|  \/  |___ __| |___| | _ )__ _| |__ _  _    | _ \___ __| |__    _ _     | _ \___| | |
//| ' \/ _ \ '_| '  \/ _` | | |_ / -_) | | | ' \/ -_) |\/| / _ | _` / -_) | _ | _` | '_ \ || |   |   / _ | _| / /   | ' \    |   / _ \ | |
//|_||_\___/_| |_|_|_\__,_|_|_/__\___| |_| |_||_\___|_|  |_\___|__,_\___|_|___|__,_|_.__/\_, |___|_|_\___|__|_\_\___|_||_|___|_|_\___/_|_|
//                                                                                       |__/|___|              |___|    |___|            
void CPlyFile5nt::normalizeTheModelBaby_Rock_n_Roll(void)
{
	// Go through all the faces, calculate the normal and 
	//	save (overwrite) the normals at that faces vertices.
	// LONG WAY version
	std::vector<PLYELEMENT>::iterator itVecFace;
	for ( itVecFace = this->m_elements.begin(); itVecFace != this->m_elements.end(); itVecFace++)
	{	// A simple, local struct to help...
		//struct SexyVector
		//{
		//	SexyVector() : x(0.0f), y(0.0f), z(0.0f) {};
		//	float x, y, z;
		//};
		// Get the three corners (verticies) of the triangle
		//SexyVector vectorA, vectorB, vectorC;
		CVector3f vectorA, vectorB, vectorC;
		vectorA.x = this->m_verticies.at( itVecFace->vertex_index_1 ).xyz.x;
		vectorA.y = this->m_verticies.at( itVecFace->vertex_index_1 ).xyz.y;
		vectorA.z = this->m_verticies.at( itVecFace->vertex_index_1 ).xyz.z;
		vectorB.x = this->m_verticies.at( itVecFace->vertex_index_2 ).xyz.x;
		vectorB.y = this->m_verticies.at( itVecFace->vertex_index_2 ).xyz.y;
		vectorB.z = this->m_verticies.at( itVecFace->vertex_index_2 ).xyz.z;
		vectorC.x = this->m_verticies.at( itVecFace->vertex_index_3 ).xyz.x;
		vectorC.y = this->m_verticies.at( itVecFace->vertex_index_3 ).xyz.y;
		vectorC.z = this->m_verticies.at( itVecFace->vertex_index_3 ).xyz.z;
		// calculate the vectors for the cross...
		//SexyVector vecAB;// = vecB - vecA
		CVector3f vecAB;
		//::VectorSubtract( vectorB.x, vectorB.y, vectorB.z,
		//	              vectorA.x, vectorA.y, vectorA.z,
		//				  vecAB.x, vecAB.y, vecAB.z );		
		vecAB = vectorB - vectorA;
		
		//SexyVector vecBC;// = vecC - vecB
		CVector3f vecBC;
		//::VectorSubtract( vectorC.x, vectorC.y, vectorC.z,
		//	              vectorB.x, vectorB.y, vectorB.z,
		//				  vecBC.x, vecBC.y, vecBC.z );
		vecBC = vectorC - vectorB;

		//SexyVector normal;
		CVector3f normal;
		//::CrossProduct( vecAB.x, vecAB.y, vecAB.z, 
		//	            vecBC.x, vecBC.y, vecBC.z, 
		//				normal.x, normal.y, normal.z );
		normal.CrossProduct( vecAB, vecBC, normal );

		//::NormalizeVector( normal.x, normal.y, normal.z );
		normal.Normalize();

		// Load the normals onto the verticies
		this->m_verticies.at( itVecFace->vertex_index_1 ).nx = normal.x;
		this->m_verticies.at( itVecFace->vertex_index_1 ).ny = normal.y;
		this->m_verticies.at( itVecFace->vertex_index_1 ).nz = normal.z;
		this->m_verticies.at( itVecFace->vertex_index_2 ).nx = normal.x;
		this->m_verticies.at( itVecFace->vertex_index_2 ).ny = normal.y;
		this->m_verticies.at( itVecFace->vertex_index_2 ).nz = normal.z;
		this->m_verticies.at( itVecFace->vertex_index_3 ).nx = normal.x;
		this->m_verticies.at( itVecFace->vertex_index_3 ).ny = normal.y;
		this->m_verticies.at( itVecFace->vertex_index_3 ).nz = normal.z;
	}
} 

// Generates texture coordinates based on a shperical projection from the 
// origin (0,0,0) location of the model. 
// NOTE: Will overwrite any existing texture coordinates 
// Inspired by: http://www.mvps.org/directx/articles/spheremap.htm
// 
//	basedOnNormals = true --> uses normals for calculation, so good for reflections
//	basedOnNormals = false --> uses model coordinates, so more 'shrink wrapped'
//  fast -> approximate or not (not uses sin() )
//  scale -> normally 1.0, but can be the limits of the texture coord
//
// The 'bias' is what coordinates are used for the generation. 
// For example, if POSITIVE_X is for u and POSITIVE_Y is for v, then:
//	u coords: based on -x to +x
//  v coords: based on -y to +y
//
void CPlyFile5nt::GenTextureCoordsSpherical( enumTEXCOORDBIAS uBias, enumTEXCOORDBIAS vBias, bool basedOnNormals, float scale, bool fast )
{
	// determine the 'remaining' coordinate...
	bool xUsed = false;
	bool yUsed = false;
	bool zUsed = false;
	if ( uBias == POSITIVE_X || vBias == POSITIVE_X )	xUsed = true;
	if ( uBias == POSITIVE_Y || vBias == POSITIVE_Y )	yUsed = true;
	if ( uBias == POSITIVE_Z || vBias == POSITIVE_Z )	yUsed = true;
	
	std::vector<PLYVERTEX>::iterator itVert;
	for ( itVert = this->m_verticies.begin(); itVert != this->m_verticies.end(); itVert++ )
	{
		CVector3f xyz;
		if ( basedOnNormals )
		{
			if ( uBias == POSITIVE_X )		xyz.x = itVert->nx;
			else if ( uBias == POSITIVE_Y )	xyz.x = itVert->ny;
			else if ( uBias == POSITIVE_Z )	xyz.x = itVert->nz;

			if ( vBias == POSITIVE_X )		xyz.y = itVert->nx;
			else if ( vBias == POSITIVE_Y )	xyz.y = itVert->ny;
			else if ( vBias == POSITIVE_Z )	xyz.y = itVert->nz;

			// Fill in the remaining coordinate...
			if ( !xUsed )	xyz.z = itVert->nx;
			if ( !yUsed )	xyz.z = itVert->ny;
			if ( !zUsed )	xyz.z = itVert->nz;
		}
		else
		{
			if ( uBias == POSITIVE_X )		xyz.x = itVert->xyz.x;
			else if ( uBias == POSITIVE_Y )	xyz.x = itVert->xyz.y;
			else if ( uBias == POSITIVE_Z )	xyz.x = itVert->xyz.z;

			if ( vBias == POSITIVE_X )		xyz.y = itVert->xyz.x;
			else if ( vBias == POSITIVE_Y )	xyz.y = itVert->xyz.y;
			else if ( vBias == POSITIVE_Z )	xyz.y = itVert->xyz.z;

			// Fill in the remaining coordinate...
			if ( !xUsed )	xyz.z = itVert->xyz.x;
			if ( !yUsed )	xyz.z = itVert->xyz.y;
			if ( !zUsed )	xyz.z = itVert->xyz.z;
		}

		xyz.Normalize();

		if ( fast )
		{
			itVert->tex0u = ( ( xyz.x / 2.0f) + 0.5f ) * scale;
			itVert->tex0v = ( ( xyz.y / 2.0f) + 0.5f ) * scale;
		}
		else
		{
			itVert->tex0u = ( ( asin(xyz.x) / PI ) + 0.5f ) * scale;
			itVert->tex0v = ( ( asin(xyz.y) / PI ) + 0.5f ) * scale;
		}
		itVert->tex1u = itVert->tex0u;
		itVert->tex1v = itVert->tex0v;
	}
}


//                         _ _        _____ _        __  __        _     _ ___      _         ___  _            _  __  ____   __            _              ___        _                ___     _ _ 
// _ _  ___ _ _ _ __  __ _| (_)______|_   _| |_  ___|  \/  |___ __| |___| | _ )__ _| |__ _  _|   \(_)_ _ ___ __| |_\ \/ /\ \ / /___ _ _ ___(_)___ _ _     | _ \___ __| |__    _ _     | _ \___| | |
//| ' \/ _ \ '_| '  \/ _` | | |_ / -_) | | | ' \/ -_) |\/| / _ | _` / -_) | _ | _` | '_ \ || | |) | | '_/ -_) _|  _|>  <  \ V // -_) '_(_-<| / _ \ ' \    |   / _ | _| / /   | ' \    |   / _ \ | |
//|_||_\___/_| |_|_|_\__,_|_|_/__\___| |_| |_||_\___|_|  |_\___|__,_\___|_|___|__,_|_.__/\_, |___/|_|_| \___\__|\__/_/\_\  \_/ \___|_| /__/|_\___/_||_|___|_|_\___|__|_\_\___|_||_|___|_|_\___/_|_|
//                                                                                       |__/                                                         |___|              |___|    |___|            
void CPlyFile5nt::normalizeTheModelBabyDirectXVersion_Rock_n_Roll(void)
{
	// Go through all the faces, calculate the normal and 
	//	save (overwrite) the normals at that faces vertices.
	// DIRECTX Version
	std::vector<PLYELEMENT>::iterator itVecFace;
	for ( itVecFace = this->m_elements.begin(); itVecFace != this->m_elements.end(); itVecFace++)
	{	
		// Get the three corners (verticies) of the triangle
		PLYVERTEX vertexA = this->m_verticies.at( itVecFace->vertex_index_1 );
		PLYVERTEX vertexB = this->m_verticies.at( itVecFace->vertex_index_2 );
		PLYVERTEX vertexC = this->m_verticies.at( itVecFace->vertex_index_3 );
		// Now calculate the vectors...
		D3DXVECTOR3 vectorA( vertexA.xyz.x, vertexA.xyz.y, vertexA.xyz.z );
		D3DXVECTOR3 vectorB( vertexB.xyz.x, vertexB.xyz.y, vertexB.xyz.z );
		D3DXVECTOR3 vectorC( vertexC.xyz.x, vertexC.xyz.y, vertexC.xyz.z );
		// calculate the vectors for the cross...
		D3DXVECTOR3 vecAB = vectorB - vectorA;
		D3DXVECTOR3 vecBC = vectorC - vectorB;
		D3DXVECTOR3 normal;
		D3DXVec3Cross( &normal, &vecAB, &vecBC );
		D3DXVec3Normalize( &normal, &normal );
		// Load the normals onto the verticies
		this->m_verticies.at( itVecFace->vertex_index_1 ).nx = normal.x;
		this->m_verticies.at( itVecFace->vertex_index_1 ).ny = normal.y;
		this->m_verticies.at( itVecFace->vertex_index_1 ).nz = normal.z;
		this->m_verticies.at( itVecFace->vertex_index_2 ).nx = normal.x;
		this->m_verticies.at( itVecFace->vertex_index_2 ).ny = normal.y;
		this->m_verticies.at( itVecFace->vertex_index_2 ).nz = normal.z;
		this->m_verticies.at( itVecFace->vertex_index_3 ).nx = normal.x;
		this->m_verticies.at( itVecFace->vertex_index_3 ).ny = normal.y;
		this->m_verticies.at( itVecFace->vertex_index_3 ).nz = normal.z;
	}
}
// ******************************************************************************************

PLYVERTEX CPlyFile5nt::getVertex_at(std::vector<PLYVERTEX>::size_type index)
{
	PLYVERTEX x = this->m_verticies.at( index );
	return x;
}

PLYELEMENT CPlyFile5nt::getElement_at(std::vector<PLYELEMENT>::size_type index)
{
	PLYELEMENT x = this->m_elements.at( index );
	return x;
}


void CPlyFile5nt::m_setIndexBasedOnPropertyName(int curIndex, std::wstring propName)
{
	if ( propName == L"x")  { this->m_x_propertyIndex = curIndex; }
	if ( propName == L"y")	{ this->m_y_propertyIndex = curIndex; }
	if ( propName == L"z")	{ this->m_z_propertyIndex = curIndex; }
	//int m_normx_propertyIndex;
	//int m_normy_propertyIndex;
	//int m_normz_propertyIndex;
	if ( propName == L"r")	{ this->m_red_propertyIndex = curIndex; }
	if ( propName == L"red")	{ this->m_red_propertyIndex = curIndex; }
	if ( propName == L"g")	{ this->m_green_propertyIndex = curIndex; }
	if ( propName == L"green")	{ this->m_green_propertyIndex = curIndex; }
	if ( propName == L"b")	{ this->m_blue_propertyIndex = curIndex; }
	if ( propName == L"blue")	{ this->m_blue_propertyIndex = curIndex; }
}

bool CPlyFile5nt::m_placeItemInAppropriateVertexLocation( std::wstring tempString, int propertyIndex, PLYVERTEX &tempVertex )
{
	// Convert val to float
	std::wstringstream ss;	
	ss << tempString;
	float valAsFloat = 0.0f;
	ss >> valAsFloat;
	if ( propertyIndex == this->m_x_propertyIndex) { tempVertex.xyz.x = valAsFloat; return true;}
	if ( propertyIndex == this->m_y_propertyIndex) { tempVertex.xyz.y = valAsFloat; return true; }
	if ( propertyIndex == this->m_z_propertyIndex) { tempVertex.xyz.z = valAsFloat; return true; }
	if ( propertyIndex == this->m_red_propertyIndex) { tempVertex.red = valAsFloat; return true; }
	if ( propertyIndex == this->m_green_propertyIndex) { tempVertex.green = valAsFloat; return true; }
	if ( propertyIndex == this->m_blue_propertyIndex) { tempVertex.blue = valAsFloat; return true; }
	// Didn't find a match
	return false;
}

int CPlyFile5nt::GetNumberOfVerticies(void)
{
	//int x = static_cast<int>( this->m_verticies.size() );
	return this->m_numberOfVertices;
}

int CPlyFile5nt::GetNumberOfElements(void)
{
	//int x = static_cast<int>( this->m_elements.size() );
	return this->m_numberOfElements;
}

float CPlyFile5nt::getMaxX(void)
{
	return this->m_maxX;
}

float CPlyFile5nt::getMinX(void)
{
	return this->m_minX;
}

float CPlyFile5nt::getDeltaX(void)
{
	return this->m_deltaX;
}

float CPlyFile5nt::getMaxY(void)
{
	return this->m_maxY;
}

float CPlyFile5nt::getMinY(void)
{
	return this->m_minY;
}

float CPlyFile5nt::getDeltaY(void)
{
	return this->m_deltaY;
}

float CPlyFile5nt::getMaxZ(void)
{
	return this->m_maxZ;
}

float CPlyFile5nt::getMinZ(void)
{
	return this->m_minZ;
}

float CPlyFile5nt::getDeltaZ(void)
{
	return this->m_deltaZ;
}

float CPlyFile5nt::getMaxExtent(void)
{
	return this->m_maxExtent;
}
