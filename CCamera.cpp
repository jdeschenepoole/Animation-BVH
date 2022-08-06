#include "CCamera.h"

CCamera::CCamera()
{
}

CCamera::~CCamera()
{
}

void CCamera::SetEyeXYZ( float x, float y, float z )
{
	this->eyeXYZ.x = x; 
	this->eyeXYZ.y = y;
	this->eyeXYZ.z = z;
	this->m_UpdateDistanceFromOrigin();
}

void CCamera::SetTargetXYZ( float x, float y, float z )
{
	this->targetXYZ.x = x;
	this->targetXYZ.y = y;
	this->targetXYZ.z = z;
	this->m_UpdateDistanceFromOrigin();
}

void CCamera::SetUpXYZ( float x, float y, float z )
{
	this->upXYZ.x = x;
	this->upXYZ.y = y;
	this->upXYZ.z = z;
}

void CCamera::OrbitRightLeft( float amount )
{
}

void CCamera::TiltUpDown( float amount )
{
}

void CCamera::m_UpdateDistanceFromOrigin( void )
{
	D3DXVECTOR3 CameraOriginRay = this->eyeXYZ - this->targetXYZ;
	this->DistanceFromOrigin = D3DXVec3Length( &CameraOriginRay );
}
