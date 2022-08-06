/* Written by Michael Feeney, Fanshawe College, (c) 2008 to 2011            */
/* mfeeney@fanshawec.ca                                                     */
/*                                                                          */
/* For the 'Game Development - Advanced Programming' curriculum             */
/* www.fanshawec.ca/en/GDP1                                                 */
/*                                                                          */
/* It may be distributed under the terms of the General Public License:     */
/* http://www.fsf.org/licenses/gpl.html                                     */
/*                                                                          */
/* Use this code at your own risk. It is indented only as a learning aid.   */
/*                                                                          */
#include "CVector3f.h"

#include <math.h>

// Fully defined in header...
//CVector3f::CVector3f() : x(0.0f), y(0.0f), z(0.0f) {};

CVector3f::CVector3f( float x, float y, float z )
{
	this->x = x; this->y = y; this->z = z;
}

CVector3f::CVector3f( const CVector3f &v )
{
	this->x = v.x; this->y = v.y; this->z = v.z;
}

// Fully defined in header...
//CVector3f::~CVector3f() {};

CVector3f& CVector3f::operator=(const CVector3f &v)
{
	// Check for self-assignment *IMPORTANT*
	if ( this == &v )	return *this;
	// Else...copy values
	this->x = v.x; this->y = v.y; this->z = v.z;
	return *this;
}

CVector3f& CVector3f::operator+(const CVector3f &v)
{
	return CVector3f( *this ) += v;
}

CVector3f& CVector3f::operator-(const CVector3f &v)
{
	return CVector3f( *this ) -= v;
}

CVector3f& CVector3f::operator+=(const CVector3f &v)
{
	this->x += v.x; this->y += v.y; this->z += v.z;
	return *this;
}

CVector3f& CVector3f::operator-=(const CVector3f &v)
{
	this->x -= v.x; this->y -= v.y; this->z -= v.z;
	return *this;
}

// Times a scalar
CVector3f& CVector3f::operator*=(float scalar)
{
	this->x *= scalar; this->y *= scalar; this->z *= scalar;
	return *this;
}

// Divided by a scalar
CVector3f& CVector3f::operator/=(float scalar)
{
	this->x /= scalar; this->y /= scalar; this->z /= scalar;
	return *this;
}
	
CVector3f CVector3f::operator-()	// Negative on all directions
{
	return CVector3f( -x, -y, -z );
}

/*static*/ // Subtracts one vector from another and returns in Sub (by reference)
void CVector3f::Subtract(const float &x0, const float &y0, const float &z0, 
                       const float &x1, const float &y1, const float &z1,
                       float &xSub, float &ySub, float &zSub)
{
	xSub = x0 - x1; ySub = y0 - y1; zSub = z0 - z1;
}

/*static*/ // Subtracts one vector from another and returns in Sub (by reference)
void CVector3f::Subtract( const CVector3f &v1, const CVector3f &v2, CVector3f &vSub )
{
	CVector3f::Subtract( v1.x, v1.y, v1.z, 
		               v2.x, v2.y, v2.z, 
					   vSub.x, vSub.y, vSub.z );
}

/*static*/ // Subtracts returns one vector from another 
CVector3f CVector3f::Subtract( const CVector3f &v1, const CVector3f &v2 )
{
	CVector3f vTemp;
	CVector3f::Subtract( v1, v2, vTemp );
	return vTemp;
}

/*static*/
CVector3f CVector3f::ScalarMultiply( const CVector3f &v, float s)
{
	CVector3f vTemp(v);
	vTemp *= s;
	return vTemp;
}

/*static*/
CVector3f CVector3f::ScalarDivide( const CVector3f &v, float s)
{
	CVector3f vTemp(v);
	vTemp /= s;
	return vTemp;
}

/*static*/
// Takes cross of vec0 (x0, y0, z0) and vec1 (x1, y1, z1) and 
//	returns cross (xCross, yCross, zCross)
void CVector3f::CrossProduct( const float &x0, const float &y0, const float &z0, 
                            const float &x1, const float &y1, const float &z1,
                            float &xCross, float &yCross, float &zCross)
{
    /* find cross-product between these vectors */
    xCross = y0 * z1 - z0 * y1;
    yCross = z0 * x1 - x0 * z1;
    zCross = x0 * y1 - y0 * x1;
}

/*static*/
void CVector3f::CrossProduct( const CVector3f v1, const CVector3f v2, CVector3f &vSub )
{
	CVector3f::CrossProduct( v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, vSub.x, vSub.y, vSub.z );
}

/*static*/
float CVector3f::DotProduct( const float &x1, const float &y1, const float &z1, 
                           const float &x2, const float &y2, const float &z2 )
{
	float dot = x1 * x2 + y1 * y2 + z1 * z2;
	return dot;
}

/*static*/
float CVector3f::DotProduct( const CVector3f &v1, const CVector3f &v2 )
{
	return DotProduct( v1.x, v1.y, v1.z, v2.x, v2.y, v2.z );
}

/*static*/	
// Set vector to unit size (length of 1.0)
void CVector3f::Normalize( float &x, float &y, float &z)
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
		x = static_cast<float>( static_cast<double>(x) * reciprical   );
		y = static_cast<float>( static_cast<double>(y) * reciprical  );
		z = static_cast<float>( static_cast<double>(z) * reciprical  );
	}
}

/*static*/	
void CVector3f::Normalize( CVector3f &v )
{
	CVector3f::Normalize( v.x, v.y, v.z );
}

void CVector3f::Normalize(void)
{
	CVector3f::Normalize( *this );
}

/*static*/
float CVector3f::Magnitude( const CVector3f &v )
{
	return static_cast<float>(sqrt( v.x*v.x + v.y*v.y + v.z*v.z ));
}

float CVector3f::Magnitude( void )
{
	return CVector3f::Magnitude( *this );
}
/*static*/
// Same as magnatude, but without the square root...
float CVector3f::MagnitudeSquared( const CVector3f &v )
{
	return static_cast<float>( v.x*v.x + v.y*v.y + v.z*v.z );

}

float CVector3f::MagnitudeSquared( void )
{
	return CVector3f::Magnitude( *this );
}

/*static*/
float CVector3f::Distance( const CVector3f &v1, const CVector3f &v2)
{
	double deltaX = static_cast<double>( v1.x - v2.x );
	double deltaY = static_cast<double>( v1.y - v2.y );
	double deltaZ = static_cast<double>( v1.z - v2.z );
	return static_cast<float>(sqrt( deltaX*deltaX + deltaY*deltaY + deltaZ*deltaZ ));
}

/*static*/
// Same as distance, but without the square root...
float CVector3f::DistanceSquared( const CVector3f &v1, const CVector3f &v2)
{
	double deltaX = static_cast<double>( v1.x - v2.x );
	double deltaY = static_cast<double>( v1.y - v2.y );
	double deltaZ = static_cast<double>( v1.z - v2.z );
	return static_cast<float>( deltaX*deltaX + deltaY*deltaY + deltaZ*deltaZ );
}

void CVector3f::Reverse(void)
{
	this->x = -(this->x);
	this->y = -(this->y);
	this->z = -(this->z);
}