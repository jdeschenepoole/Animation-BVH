#ifndef _CCamera_HG_
#define _CCamera_HG_

#include "D3DX10.h"	// For the D3DXVECTOR4

class CCamera
{
public:
	CCamera();
	virtual ~CCamera();
	D3DXVECTOR3 eyeXYZ;		
	D3DXVECTOR3 targetXYZ;	
	D3DXVECTOR3 upXYZ;

	void SetEyeXYZ( float x, float y, float z );
	void SetTargetXYZ( float x, float y, float z );
	void SetUpXYZ( float x, float y, float z );

	void OrbitRightLeft( float amount );
	void TiltUpDown( float amount );

private:
	// NOTE: This is updated by updating the eye or target
	void m_UpdateDistanceFromOrigin( void );
	float DistanceFromOrigin;
};

#endif