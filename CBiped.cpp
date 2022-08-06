#include "CBiped.h"
#include <fstream>
#include <algorithm>
#include <sstream>	// for the error handling

#include "globalAngleAdjust.h"

CBiped::CBiped()
{
	this->m_pObjectManager = 0;
	this->m_CurrentFrame = 0.0f;
}

CBiped::~CBiped()
{
}

void CBiped::SetObjectManager( CObjectManager* pTheObjectManager )
{
	this->m_pObjectManager = pTheObjectManager;
}

// Passes:
//	* BVHFileName
//  * loaded map of BVH nodes to ply infor + offsets, etc.
//  * Error returns any error codes
bool CBiped::CreateBiped( std::wstring BVHFileName,  std::wstring BVHToPLYInfoFile, std::wstring &error )
{
	bool AllOK = true;

	int errorCode = 0; 
	if ( !this->m_theBVHFile.LoadBVHFile( BVHFileName, errorCode ) )
	{
		error = this->m_theBVHFile.TranslateErrorCode( errorCode );
		return false;
	}

	if ( !this->m_ProcessBVHPLYInfoFile( BVHToPLYInfoFile, error ) )
	{
		return false;
	}

	// **********************************************************
	// ADDED....
	// method that loads above map
	if ( !this->m_loadMapToJointPointers( error ) )
	{	// Something is wrong
		return false;
	}


	// Good to go...
	return AllOK; 
}


// method that loads above map
bool CBiped::m_loadMapToJointPointers( std::wstring &error )
{
	std::map< std::wstring, CBipedPartPLY >::iterator itBVHJointName;
	for ( itBVHJointName = this->m_mapBVHPlyMatchup.begin(); 
		  itBVHJointName != this->m_mapBVHPlyMatchup.end(); itBVHJointName++ )
	{
		//itBVHJoint
		CObject* pJoint = this->m_findCObjectByName( itBVHJointName->first, this->m_rootObject );
		if ( pJoint == 0 )
		{	// Something is wrong.
			std::wstringstream ss;
			ss << L"ERROR: Can't find " << itBVHJointName->first << " in node list.";
			return false;
		}
		// Found it.
		this->m_mapJointToJointPointers[ itBVHJointName->first ] = pJoint;
	}

	return true;
}

CObject* CBiped::m_findCObjectByName( std::wstring jointName, CObject &theNode )
{
	if ( theNode.name == jointName )
	{
		return &theNode;
	}
	// It's not the root object, so keep searching
	for ( std::vector< CObject >::iterator itChildNode = theNode.vecChildObjects.begin();
		  itChildNode != theNode.vecChildObjects.end(); itChildNode++ )
	{
		CObject* pNode = this->m_findCObjectByName( jointName, *itChildNode );
		if ( pNode != 0 )
		{	// It's the one!
			return pNode;
		}
	}

	return 0;
}


//bool CBiped::LoadNewBVH( std::wstring BVHFileName, std::wstring &error )
//{
//	bool AllOK = true;
//
//	CBVHFile theBVHFile; 
//	int errorCode = 0; 
//	if ( !theBVHFile.LoadBVHFile( BVHFileName, errorCode ) )
//	{
//		error = theBVHFile.TranslateErrorCode( errorCode );
//		return false;
//	}
//
//	return this->m_ProcessBVHPLYInfoFile( BVHFileName, error);;
//}

bool CBiped::m_ProcessBVHPLYInfoFile( std::wstring BVHPLYInfoFile, std::wstring &error )
{
	// Try to open the file...
	std::wifstream theFile( BVHPLYInfoFile.c_str() );
	if ( !theFile.is_open() )
	{
		error = L"Can't open BHV-PLY-Info file.";
		return false;
	}

	// Process it. 
	// Format:
	// BVHJoint   PLYFile   OffsetAdjustXYZ    RotationAdjustXYZ

	std::wstring BVHPartName;
	// Will keep reading until there are no more data...
	while ( theFile >> BVHPartName )
	{
		CBipedPartPLY temp;

		theFile >> temp.PlyFileName;
		theFile >> temp.OffsetAdjust.x >> temp.OffsetAdjust.y >> temp.OffsetAdjust.z;
		theFile >> temp.RotationAdjust.x >> temp.RotationAdjust.y >> temp.RotationAdjust.z;

		// Add to map...
		this->m_mapBVHPlyMatchup[ BVHPartName ] = temp;
	};

	// At this point, we know what ply files we need to load, and the 
	//	adjustment information that we need.
	// Now, we need to create a vector of ply files so the object manager can load it
	// (in other words, we don't want any duplicates...)
	std::map< std::wstring, CBipedPartPLY >::iterator itBVHPLYInfo;
	for ( itBVHPLYInfo = this->m_mapBVHPlyMatchup.begin(); 
		  itBVHPLYInfo != this->m_mapBVHPlyMatchup.end(); itBVHPLYInfo++ )
	{
		// Is this ply in the vector already? 
		std::wstring plyFileToFind = (*itBVHPLYInfo).second.PlyFileName;
		std::vector< std::wstring >::iterator itVec = 
			std::find( this->PlyFilesWeAreUsing.begin(), 
			           this->PlyFilesWeAreUsing.end(), plyFileToFind );
		if ( itVec == this->PlyFilesWeAreUsing.end() )
		{	// nope, not there, so add it
			this->PlyFilesWeAreUsing.push_back( plyFileToFind );
		}
	}

	// This part creates the tree of nodes (CObjects) that will make up the model
	this->m_rootObject.translationXYZ = this->m_mapBVHPlyMatchup[L"hip"].OffsetAdjust;
	this->m_rootObject.preRotationXYZ = this->m_mapBVHPlyMatchup[L"hip"].RotationAdjust;
	this->m_rootObject.plyFileName = this->m_mapBVHPlyMatchup[L"hip"].PlyFileName;
	this->m_rootObject.material = ::getRandomMaterial();
	this->m_rootObject.name = L"hip";

	CObject abdomen;
	abdomen.translationXYZ = this->m_mapBVHPlyMatchup[L"abdomen"].OffsetAdjust;
	abdomen.preRotationXYZ = this->m_mapBVHPlyMatchup[L"abdomen"].RotationAdjust;
	abdomen.plyFileName = this->m_mapBVHPlyMatchup[L"abdomen"].PlyFileName;
	abdomen.material = ::getRandomMaterial();
	abdomen.bIsRootNodeSetToOrigin = false;
	abdomen.name = L"abdomen";
	
	CObject chest;
	chest.translationXYZ = this->m_mapBVHPlyMatchup[L"chest"].OffsetAdjust;
	chest.preRotationXYZ = this->m_mapBVHPlyMatchup[L"chest"].RotationAdjust;
	chest.plyFileName = this->m_mapBVHPlyMatchup[L"chest"].PlyFileName;
	chest.material = ::getRandomMaterial();
	chest.bIsRootNodeSetToOrigin = false;
	chest.name = L"chest";

	
	CObject neck;
	neck.translationXYZ = this->m_mapBVHPlyMatchup[L"neck"].OffsetAdjust;
	neck.preRotationXYZ = this->m_mapBVHPlyMatchup[L"neck"].RotationAdjust;
	neck.plyFileName = this->m_mapBVHPlyMatchup[L"neck"].PlyFileName;
	neck.material = ::getRandomMaterial();
	neck.bIsRootNodeSetToOrigin = false;
	neck.name = L"neck";

	CObject head;
	head.translationXYZ = this->m_mapBVHPlyMatchup[L"head"].OffsetAdjust;
	head.preRotationXYZ = this->m_mapBVHPlyMatchup[L"head"].RotationAdjust;
	head.plyFileName = this->m_mapBVHPlyMatchup[L"head"].PlyFileName;
	head.material = ::getRandomMaterial();
	head.bIsRootNodeSetToOrigin = false;
	head.name = L"head";

	CObject lCollar;
	lCollar.translationXYZ = this->m_mapBVHPlyMatchup[L"lCollar"].OffsetAdjust;
	lCollar.preRotationXYZ = this->m_mapBVHPlyMatchup[L"lCollar"].RotationAdjust;
	lCollar.plyFileName = this->m_mapBVHPlyMatchup[L"lCollar"].PlyFileName;
	lCollar.material = ::getRandomMaterial();
	lCollar.bIsRootNodeSetToOrigin = false;
	lCollar.name = L"lCollar";

	CObject rCollar;
	rCollar.translationXYZ = this->m_mapBVHPlyMatchup[L"rCollar"].OffsetAdjust;
	rCollar.preRotationXYZ = this->m_mapBVHPlyMatchup[L"rCollar"].RotationAdjust;
	rCollar.plyFileName = this->m_mapBVHPlyMatchup[L"rCollar"].PlyFileName;
	rCollar.material = ::getRandomMaterial();
	rCollar.bIsRootNodeSetToOrigin = false;
	rCollar.name = L"rCollar";

	CObject lShldr;
	lShldr.translationXYZ = this->m_mapBVHPlyMatchup[L"lShldr"].OffsetAdjust;
	lShldr.preRotationXYZ = this->m_mapBVHPlyMatchup[L"lShldr"].RotationAdjust;
	lShldr.plyFileName = this->m_mapBVHPlyMatchup[L"lShldr"].PlyFileName;
	lShldr.material = ::getRandomMaterial();
	lShldr.bIsRootNodeSetToOrigin = false;
	lShldr.name = L"lShldr";

	CObject rShldr;
	rShldr.translationXYZ = this->m_mapBVHPlyMatchup[L"rShldr"].OffsetAdjust;
	rShldr.preRotationXYZ = this->m_mapBVHPlyMatchup[L"rShldr"].RotationAdjust;
	rShldr.plyFileName = this->m_mapBVHPlyMatchup[L"rShldr"].PlyFileName;
	rShldr.material = ::getRandomMaterial();
	rShldr.bIsRootNodeSetToOrigin = false;
	rShldr.name = L"rShldr";

	CObject lForeArm;
	lForeArm.translationXYZ = this->m_mapBVHPlyMatchup[L"lForeArm"].OffsetAdjust;
	lForeArm.preRotationXYZ = this->m_mapBVHPlyMatchup[L"lForeArm"].RotationAdjust;
	lForeArm.plyFileName = this->m_mapBVHPlyMatchup[L"lForeArm"].PlyFileName;
	lForeArm.material = ::getRandomMaterial();
	lForeArm.bIsRootNodeSetToOrigin = false;
	lForeArm.name = L"lForeArm";	
	
	CObject rForeArm;
	rForeArm.translationXYZ = this->m_mapBVHPlyMatchup[L"rForeArm"].OffsetAdjust;
	rForeArm.preRotationXYZ = this->m_mapBVHPlyMatchup[L"rForeArm"].RotationAdjust;
	rForeArm.plyFileName = this->m_mapBVHPlyMatchup[L"rForeArm"].PlyFileName;
	rForeArm.material = ::getRandomMaterial();
	rForeArm.bIsRootNodeSetToOrigin = false;
	rForeArm.name = L"rForeArm";	

	CObject lHand;
	lHand.translationXYZ = this->m_mapBVHPlyMatchup[L"lHand"].OffsetAdjust;
	lHand.preRotationXYZ = this->m_mapBVHPlyMatchup[L"lHand"].RotationAdjust;
	lHand.plyFileName = this->m_mapBVHPlyMatchup[L"lHand"].PlyFileName;
	lHand.material = ::getRandomMaterial();
	lHand.bIsRootNodeSetToOrigin = false;
	lHand.name = L"lHand";

	CObject rHand;
	rHand.translationXYZ = this->m_mapBVHPlyMatchup[L"rHand"].OffsetAdjust;
	rHand.preRotationXYZ = this->m_mapBVHPlyMatchup[L"rHand"].RotationAdjust;
	rHand.plyFileName = this->m_mapBVHPlyMatchup[L"rHand"].PlyFileName;
	rHand.material = ::getRandomMaterial();
	rHand.bIsRootNodeSetToOrigin = false;
	rHand.name = L"rHand";	

	CObject lThigh;
	lThigh.translationXYZ = this->m_mapBVHPlyMatchup[L"lThigh"].OffsetAdjust;
	lThigh.preRotationXYZ = this->m_mapBVHPlyMatchup[L"lThigh"].RotationAdjust;
	lThigh.plyFileName = this->m_mapBVHPlyMatchup[L"lThigh"].PlyFileName;
	lThigh.material = ::getRandomMaterial();
	lThigh.bIsRootNodeSetToOrigin = false;
	lThigh.name = L"lThigh";	

	CObject rThigh;
	rThigh.translationXYZ = this->m_mapBVHPlyMatchup[L"rThigh"].OffsetAdjust;
	rThigh.preRotationXYZ = this->m_mapBVHPlyMatchup[L"rThigh"].RotationAdjust;
	rThigh.plyFileName = this->m_mapBVHPlyMatchup[L"rThigh"].PlyFileName;
	rThigh.material = ::getRandomMaterial();
	rThigh.bIsRootNodeSetToOrigin = false;
	rThigh.name = L"rThigh";	

	CObject lShin;
	lShin.translationXYZ = this->m_mapBVHPlyMatchup[L"lShin"].OffsetAdjust;
	lShin.preRotationXYZ = this->m_mapBVHPlyMatchup[L"lShin"].RotationAdjust;
	lShin.plyFileName = this->m_mapBVHPlyMatchup[L"lShin"].PlyFileName;
	lShin.material = ::getRandomMaterial();
	lShin.bIsRootNodeSetToOrigin = false;
	lShin.name = L"lShin";	

	CObject rShin;
	rShin.translationXYZ = this->m_mapBVHPlyMatchup[L"rShin"].OffsetAdjust;
	rShin.preRotationXYZ = this->m_mapBVHPlyMatchup[L"rShin"].RotationAdjust;
	rShin.plyFileName = this->m_mapBVHPlyMatchup[L"rShin"].PlyFileName;
	rShin.material = ::getRandomMaterial();
	rShin.bIsRootNodeSetToOrigin = false;
	rShin.name = L"rShin";	

	CObject rFoot;
	rFoot.translationXYZ = this->m_mapBVHPlyMatchup[L"rFoot"].OffsetAdjust;
	rFoot.preRotationXYZ = this->m_mapBVHPlyMatchup[L"rFoot"].RotationAdjust;
	rFoot.plyFileName = this->m_mapBVHPlyMatchup[L"rFoot"].PlyFileName;
	rFoot.material = ::getRandomMaterial();
	rFoot.bIsRootNodeSetToOrigin = false;
	rFoot.name = L"rFoot";	

	CObject lFoot;
	lFoot.translationXYZ = this->m_mapBVHPlyMatchup[L"lFoot"].OffsetAdjust;
	lFoot.preRotationXYZ = this->m_mapBVHPlyMatchup[L"lFoot"].RotationAdjust;
	lFoot.plyFileName = this->m_mapBVHPlyMatchup[L"lFoot"].PlyFileName;
	lFoot.material = ::getRandomMaterial();
	lFoot.bIsRootNodeSetToOrigin = false;
	lFoot.name = L"lFoot";	

	lShin.vecChildObjects.push_back( lFoot );
	rShin.vecChildObjects.push_back( rFoot );

	lThigh.vecChildObjects.push_back( lShin );
	rThigh.vecChildObjects.push_back( rShin );

	neck.vecChildObjects.push_back( head );

	lForeArm.vecChildObjects.push_back( lHand );
	rForeArm.vecChildObjects.push_back( rHand );

	lShldr.vecChildObjects.push_back( lForeArm );
	rShldr.vecChildObjects.push_back( rForeArm );

	lCollar.vecChildObjects.push_back( lShldr );
	rCollar.vecChildObjects.push_back( rShldr );

	chest.vecChildObjects.push_back( neck );
	chest.vecChildObjects.push_back( lCollar );
	chest.vecChildObjects.push_back( rCollar );

	abdomen.vecChildObjects.push_back( chest );

	// Add abdomen & thighs to hip...
	this->m_rootObject.vecChildObjects.push_back(abdomen);
	this->m_rootObject.vecChildObjects.push_back( lThigh );
	this->m_rootObject.vecChildObjects.push_back( rThigh );

	return true;
}

bool CBiped::GetRootNodeObject( CObject &rootObject, std::wstring &error )
{
	rootObject = this->m_rootObject;
	return true;
}

// This sets the next frame. Note that it's a float...
// Updates all the objects in the tree to new positions based
//	on the frame and BVH data...
bool CBiped::IncrementFrame( float secondsToUpdate, bool loopFrame )
{
	// Now many frames for that may seconds?
	float framesToIncrement = secondsToUpdate / this->m_theBVHFile.frameTime;

	float tempCurrentFrame = this->getCurrentFrame();
	if (::reverseAnimation == true)
		tempCurrentFrame -= framesToIncrement;
	else
		tempCurrentFrame += framesToIncrement;
	// Gone too far? 
	if ( tempCurrentFrame > this->m_theBVHFile.numberOfFrames )
	{	// Yup.
		if ( loopFrame )
		{
			tempCurrentFrame = 0.0f;
		}
		else
		{	// Clamp it
			tempCurrentFrame = static_cast<float>( this->m_theBVHFile.numberOfFrames - 1 );
		}
	}
	else if (tempCurrentFrame < 0.0f)
	{
		if ( loopFrame )
		{
			tempCurrentFrame = static_cast<float>( this->m_theBVHFile.numberOfFrames - 1);
		}
		else
		{	// Clamp it
			tempCurrentFrame = 0.0f;
		}
	}
	// Go through the list of objects, updating the positions (angles, really)
	//	based on the BVH data...

	if ( !this->setCurrentFrame( tempCurrentFrame ) )
	{	// Something is wrong.
		return false;
	}

	return true;
}

// ADDED, November 30, 2011
bool CBiped::SetJointAngle( std::wstring jointName, CVector3f angleXYZ, bool bInDegrees )
{
	//hip, abdomen, chest, neck, head, 
	//lCollar, lShldr, lForeArm, lHand, 
	//rCollar, rShldr, rForeArm, rHand
	//lThigh, lShin, lFoot
	//rThigh, rShin, rFoot	

	// Find the corresponding joint...
	CObject* pObject = this->m_findCObjectByName( jointName, this->m_rootObject );
	if ( pObject == 0 )
	{	// Didn't find it.
		return false;
	}

	// This is "good enough for rock-n-roll" but here we would make some tweak to 
	//	"fix" the angles that are all screwy. Note that you could (should?) place this
	//	sort of information into some external file that would be looked up and 
	//	made specific for each BVH file, or you would "process" the files first, fixing 
	//	any problems before you load them.
	if ( jointName == L"hip" )
	{
		angleXYZ.y -= 90.0f;
	}
	else if ( jointName == L"abdomen" )
	{	
		angleXYZ += g_AngleAdjust;
		
	}
	else if ( jointName == L"chest" )
	{
	}
	else if ( jointName == L"neck" )
	{
		angleXYZ.x += 90.0f;
	}
	else if ( jointName == L"head" )
	{
	}
	else if ( jointName == L"lCollar" )
	{
	}
	else if ( jointName == L"lShldr" )
	{
		
	}
	else if ( jointName == L"rShldr" )
	{
		angleXYZ.y += 180.0f;
	}
	else if ( jointName == L"lForeArm" )
	{
	}
	else if ( jointName == L"lHand" )
	{
	}
	else if ( jointName == L"rCollar" )
	{
	}
	else if ( jointName == L"rShldr" )
	{
		angleXYZ += g_AngleAdjust;
	}
	else if ( jointName == L"rForeArm" )
	{
	}
	else if ( jointName == L"rHand" )
	{
	}
	else if ( jointName == L"lThigh" )
	{
		angleXYZ.y += 270.0f;
		angleXYZ.z += 45.0f;
	}
	else if ( jointName == L"lShin" )
	{
	}
	else if ( jointName == L"lFoot" )
	{
	}
	else if ( jointName == L"rThigh" )
	{
		angleXYZ.y += 270.0f;
		angleXYZ.z += 45.0f;
	}
	else if ( jointName == L"rShin" )
	{
	}
	else if ( jointName == L"rFoot" )
	{
	}

	const float PI_Over_180 = 3.14159f / 180.0f;

	// Are we passing degrees? 
	if ( bInDegrees )
	{
		angleXYZ.x *= PI_Over_180;
		angleXYZ.y *= PI_Over_180;
		angleXYZ.z *= PI_Over_180;
	}

	// Now set the angle for that CObject
	pObject->preRotationXYZ = angleXYZ;
	
	return true;
}

// ADDED: November 30, 2011
float CBiped::getCurrentFrame( void )
{
	return this->m_CurrentFrame;
}

// ADDED: November 30, 2011
bool CBiped::setCurrentFrame( float newFrameNumber )
{
	this->m_CurrentFrame = newFrameNumber;
	// Convert to an integer because the BVH only has integer frame info.
	int iCurrentFrame = static_cast<int>( this->m_CurrentFrame );
	int iNextFrame; 

	// Gets the fractional part
	float fraction = 0.0f;
	//float fCurrentFrame = (float) iCurrentFrame;
	fraction = newFrameNumber - iCurrentFrame;

	if (::reverseAnimation)
		iNextFrame = iCurrentFrame - 1;
	else
		iNextFrame = iCurrentFrame + 1;

	if (iNextFrame < 0)
	{
		iNextFrame = 0;
		fraction = 0.0f;
	}
	else if (iNextFrame > this->m_theBVHFile.numberOfFrames - 1)
	{
		iNextFrame = this->m_theBVHFile.numberOfFrames - 1;
		fraction = 0.0f;
	}



	// Now update the model...
	// Here, we go throught the BVH to Ply information map, reading each joint, etc.
	// (note: this also allows us to ignore BVH joints that we don't need
	std::map< std::wstring, CBipedPartPLY >::iterator itBVHJoint;
	for ( itBVHJoint = this->m_mapBVHPlyMatchup.begin(); itBVHJoint != this->m_mapBVHPlyMatchup.end(); itBVHJoint++ )
	{
		// Get three joint angles...
		CVector3f newAngle;
		CVector3f initialAngleAdjust = itBVHJoint->second.RotationAdjust;
		std::wstring jointName = itBVHJoint->first;
		// Now find it in the BVH information...
		if ( !this->m_theBVHFile.getAngleInfo( jointName, L"Xrotation", iCurrentFrame, newAngle.x ) )
		{	// Error: Can't find it
			return false;
		}
		if ( !this->m_theBVHFile.getAngleInfo( jointName, L"Yrotation", iCurrentFrame, newAngle.y ) )
		{	// Error: Can't find it
			return false;
		}
		if ( !this->m_theBVHFile.getAngleInfo( jointName, L"Zrotation", iCurrentFrame, newAngle.z ) )
		{	// Error: Can't find it
			return false;
		}

		// Get three joint angles...
		CVector3f nextAngle;
		// Now find it in the BVH information...
		if ( !this->m_theBVHFile.getAngleInfo( jointName, L"Xrotation", iNextFrame, nextAngle.x ) )
		{	// Error: Can't find it
			return false;
		}
		if ( !this->m_theBVHFile.getAngleInfo( jointName, L"Yrotation", iNextFrame, nextAngle.y ) )
		{	// Error: Can't find it
			return false;
		}
		if ( !this->m_theBVHFile.getAngleInfo( jointName, L"Zrotation", iNextFrame, nextAngle.z ) )
		{	// Error: Can't find it
			return false;
		}
		// Now we have the information, so we set it in the graphical model (the tree of CObjects)
		
		// First, we add the intial offsets for the model (these may have to be changed in the BVH_PLY_INFO.txt file)
		newAngle = initialAngleAdjust + newAngle;
		nextAngle = initialAngleAdjust + (nextAngle - newAngle);

		nextAngle.x *= fraction;
		nextAngle.y *= fraction;
		nextAngle.z *= fraction;

		CVector3f finalAngle;
		if (::reverseAnimation)
			finalAngle = newAngle + nextAngle;
		else
			finalAngle = newAngle - nextAngle;

		// And set them...(in degrees)
		this->SetJointAngle( jointName, finalAngle, true );
	}
	// And we're done.
	return true;
}

// #Checkpoint3.1
void CBiped::moveBiped(float x, float y, float z)
{
	this->m_rootObject.translationXYZ.x += x;
	this->m_rootObject.translationXYZ.y += y;
	this->m_rootObject.translationXYZ.z += z;
}