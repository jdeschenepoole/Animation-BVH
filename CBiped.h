#ifndef _CBiped_HG_
#define _CBiped_HG_

#include "CObjectManager.h"
#include <map>
#include <vector>
#include "CBVHFile.h"

class CBipedPartPLY
{
public:
	std::wstring PlyFileName;
	CVector3f OffsetAdjust;
	CVector3f RotationAdjust;
};

class CBiped
{
public:
	CBiped();
	virtual ~CBiped();

	void SetObjectManager( CObjectManager* pTheObjectManager );

	// Loads a BVH file, creates the drawing objects and loads the ply information
	// It creates the tree of nodes based on the BVHToPLYInfoFile and the BVHFile
	bool CreateBiped( std::wstring BVHFileName, std::wstring BVHToPLYInfoFile, std::wstring &error );
	std::vector< std::wstring > PlyFilesWeAreUsing;
	
	// This is used by the renderer to get the things to draw. 
	bool GetRootNodeObject( CObject &rootObject, std::wstring &error );

	// This sets the next frame. Note that it's a float...
	// ADDED: November 30, 2011 - the loopFrame boolean value
	bool IncrementFrame( float secondsToUpdate, bool loopFrame );
	// ADDED: November 30, 2011
	float getCurrentFrame( void );
	// ADDED: November 30, 2011
	// This one does a lot of work, getting the angle info from the BVH, then 
	//	calling m_SetJointAngle on all the objects. 
	bool setCurrentFrame( float newFrameNumber );	
	// Added, November 30, 2011 
	// This sets the angles of the joints on the model.
	bool SetJointAngle( std::wstring jointName, CVector3f angleXYZ, bool bInDegrees );

	void moveBiped(float x, float y, float z);

	//bool LoadNewBVH( std::wstring BVHFileName, std::wstring &error );
private:
	CObjectManager* m_pObjectManager;
	bool m_ProcessBVHPLYInfoFile( std::wstring BVHPLYInfoFile, std::wstring &error );
	std::map< std::wstring, CBipedPartPLY > m_mapBVHPlyMatchup;
	CBVHFile m_theBVHFile;
	CObject m_rootObject;

	// ******************************************************************
	// Added, November 30, 2011 
	// map that links the joint names with points to the actual joints
	std::map< std::wstring, CObject* > m_mapJointToJointPointers;
	// method that loads above map
	bool m_loadMapToJointPointers( std::wstring &error );
	// This recursively looks for the object by name...
	CObject* m_findCObjectByName( std::wstring jointName, CObject &theNode );
	// 
	float m_CurrentFrame;
};

#endif