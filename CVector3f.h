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
#ifndef _CVector3f_HG_
#define _CVector3f_HG_

class CVector3f
{
public:
	CVector3f() : x(0.0f), y(0.0f), z(0.0f) {};
	CVector3f( float x, float y, float z );
	CVector3f( const CVector3f &v );	// Copy constructor
	virtual ~CVector3f() {};
	float x; float y; float z;

	// These are used on vector types
	CVector3f& operator=(const CVector3f &v);
	CVector3f& operator+(const CVector3f &v);
	CVector3f& operator-(const CVector3f &v);
	CVector3f& operator+=(const CVector3f &v);
	CVector3f& operator-=(const CVector3f &v);
	CVector3f& operator*=(float scalar);	// Times a scalar
	CVector3f& operator/=(float scalar);	// Divided by a scalar
	CVector3f operator-();	// Negative on all directions
	
	// Helpful functions
	void Normalize(void);
	float Magnitude(void);
	float MagnitudeSquared(void);
	void Reverse(void);
	
	// Helpful static functions...
	static void Normalize( float &x, float &y, float &z);
	static void Normalize( CVector3f &v );
	static float Magnitude( const CVector3f &v );
	static float MagnitudeSquared( const CVector3f &v );
	static float Distance( const CVector3f &v1, const CVector3f &v2);
	static float DistanceSquared( const CVector3f &v1, const CVector3f &v2);

	static void Subtract( const float &x0, const float &y0, const float &z0, 
                          const float &x1, const float &y1, const float &z1,
                          float &xSub, float &ySub, float &zSub);
	static void Subtract( const CVector3f &v1, const CVector3f &v2, CVector3f &vSub );
	static CVector3f Subtract( const CVector3f &v1, const CVector3f &v2 );
	static CVector3f ScalarMultiply( const CVector3f &v, float s);
	static CVector3f ScalarDivide( const CVector3f &v, float s);

	static void CrossProduct( const float &x0, const float &y0, const float &z0, 
	                          const float &x1, const float &y1, const float &z1,
	                          float &xCross, float &yCross, float &zCross);
	static void CrossProduct( const CVector3f v1, const CVector3f v2, CVector3f &vSub );

	static float DotProduct( const float &x1, const float &y1, const float &z1, 
	                         const float &x2, const float &y2, const float &z2 );
	static float DotProduct( const CVector3f &v1, const CVector3f &v2 );
};

#endif