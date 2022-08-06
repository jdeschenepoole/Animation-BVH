#ifndef _VariousTypes_HG_
#define _VariousTypes_HG_

#include <d3dx10.h>			// for D3DXVECTOR4
#include "CVector3f.h"

struct MaterialInfo
{
    D3DXVECTOR4 diffuse;		// float4 in shader
	D3DXVECTOR4 ambient;
    D3DXVECTOR4 spec;
};

struct LightDesc
{
	D3DXVECTOR4 pos;
	D3DXVECTOR4 dir;
	MaterialInfo material;	// ambient, diffuse, spec
	D3DXVECTOR4 attenuation;
	D3DXVECTOR4 spotPowerRange;	// only first two floats are used x = power, y = range
};

MaterialInfo getRandomMaterial(void);

#endif