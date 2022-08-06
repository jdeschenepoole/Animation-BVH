#ifndef _CBVHFile_HG_
#define _CBVHFile_HG_

#include <iostream>
#include <map>

#include "CBVHJoint.h"

class CBVHFile
{
public:
	CBVHFile();
	virtual ~CBVHFile();
	bool LoadBVHFile( std::wstring fileName, int &errorCode );
	std::wstring TranslateErrorCode( int errorCode );
	CBVHJoint rootJoint;
	//
	int numberOfFrames;
	float frameTime;
	float getCaptureTime( void );		// frameTime * numberOfFrames
	float getFrameInfo( std::wstring jointName, int frameNumber );	// Returns a particular frame of a particular joint
	float getFrameInfo( int channelNumber, int frameNumber );		// Returns a particular frame from a particular channel
	// ADDED: November 30, 2011
	// Returns a particular frame of a particular joint
	bool getAngleInfo( std::wstring jointName, std::wstring channelName, int frameNumber, float &angle );	
	// ADDED: November 30, 2011
	bool FindJointByName( std::wstring jointName, CBVHJoint &theJoint );
private:
	// The raw channel data, each containinig the frame data (vector index = frame number)
	std::vector< std::vector< float > > vecAllChannelData;	// Contained vector is the frame for each channel

	// Used during parsing. Finds the channel number in the joints, then copies the data to it...
	bool m_CopyFrameInfoToJoint( int channelNumber, std::vector< float > vecFrameData,
								 std::vector< CBVHJoint >::iterator itJointBegin, 
								 std::vector< CBVHJoint >::iterator itJointEnd );

	// This maps the channel information from the hierarchy to the motion portion
	// The value is the column in the motion portion... is used when parsing the data
	unsigned int currentMotionOffset;		// Indicates the number of channels (one more than the index)
	//bool m_FindJoint( int channelNumber, CBVHChannel &channel );		// Finds channel (in joint) by channel number

	// ADDED: November 30, 2011
	// This maps the joint+channel name (one string) to the channel number
	std::map< std::wstring, int > m_mapJointChannelNameToChannelNumber;
	bool m_CreateJointMap( void );
	bool AddJointChannelNameToMap( CBVHJoint &theJoint );
};

#endif