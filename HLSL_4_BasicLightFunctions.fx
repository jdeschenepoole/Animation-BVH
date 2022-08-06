// mfeeney@fanshawec.ca
// The lighting functions are (modified) from Frank Luna's DirectX 10 book (chapter 6)

// This is used to specify what the material property of the light and objects are
struct MaterialInfo
{
    float4 diffuse;
	float4 ambient;
    float4 spec;
};

struct LightDesc
{
	float4 pos;
	float4 dir;
	MaterialInfo material;	// ambient, diffuse, spec
	float4 attenuation;
	float4 spotPowerRange;	// only first two floats are used x = power, y = range
};

// v = material description of the surface
// L = lighting description, including material
// pos = world space vertex position
// normal = world space vertex normal
// eyePos = camera (eye) world position
float4 PointLight(MaterialInfo v, LightDesc L, float4 pos, float4 normal, float4 eyePos)
{
	float3 litColor = float3(0.0f, 0.0f, 0.0f);

	// The vector from the surface to the light.
	float3 lightPos = L.pos.xyz;
	float3 vertexPos = pos.xyz;
	float3 lightVec = lightPos - vertexPos;
		//
	// The distance from surface to light.
	float d = length(lightVec);

	if( d > L.spotPowerRange.y )			// y element is range of spot or point light
	{
		return float4(0.0f, 0.0f, 0.0f, 1.0f);		// If it's too far, set colour to black.
		// Note: since it's black, we don't need to to any more calculations.
	}

	// Normalize the light vector.
	lightVec /= d;			//normalize( lightVec );

	// Add the ambient light term.
	// 	litColor += v.diffuse.xyz * L.material.ambient.xyz;
	litColor += v.diffuse.rgb * L.material.ambient.rgb;			// Same as above: xyzw same as rgba

	// Add diffuse and specular term, provided the surface is in 
	// the line of site of the light.

	float diffuseFactor = dot(lightVec, normal.xyz);
	[branch]
	if( diffuseFactor > 0.0f )
	{
		float specPower  = max(v.spec.a, 1.0f);
		float3 toEye     = normalize( eyePos.xyz - vertexPos.xyz );
		float3 R         = reflect( -lightVec, normal.xyz );
		float specFactor = pow(max(dot(R, toEye), 0.0f), specPower);

		// diffuse and specular terms
		litColor += diffuseFactor * v.diffuse * L.material.diffuse;
		litColor += specFactor * v.spec * L.material.spec;
	}
	
	// attenuate
	litColor =  litColor / dot( L.attenuation, float3(1.0f, d, d*d));

	return float4( litColor.xyz, 1.0f );
}

//float3 ParallelLight( vSurfaceInfo, Light L, float3 eyePos)
float4 ParallelLight( MaterialInfo v, LightDesc L, float4 pos, float4 normal, float4 eyePos )
{
	float3 litColor = float3(0.0f, 0.0f, 0.0f);
 
	// The light vector aims opposite the direction the light rays travel.
	float3 lightVec = -L.dir.xyz;
	
	// Add the ambient term.
	litColor += v.diffuse.rgb * L.material.ambient.rgb;	
	
	// Add diffuse and specular term, provided the surface is in 
	// the line of site of the light.
	
	float diffuseFactor = dot( lightVec, normal.xyz );

	[branch]
	if( diffuseFactor > 0.0f )
	{
		float specPower  = max(v.spec.a, 1.0f);
		float3 toEye     = normalize( eyePos.xyz - pos );
		float3 R         = reflect( lightVec, normal.xyz );
		float specFactor = pow(max(dot(R, toEye), 0.0f), specPower);
					
		

		// diffuse and specular terms
		litColor += diffuseFactor * v.diffuse * L.material.diffuse;
		//litColor += specFactor * v.spec * L.material.spec;
	}
	
	return float4( litColor.rgb, 1.0f );
}

//float3 Spotlight(SurfaceInfo v, Light L, float3 eyePos)
float4 Spotlight( MaterialInfo v, LightDesc L, float4 pos, float4 normal, float4 eyePos )
{
	float4 litColor = PointLight(v, L, pos, normal, eyePos);
	
	// The vector from the surface to the light.
	float3 lightVec = normalize(L.pos.xyz - pos.xyz);
	
	float s = pow(max(dot(-lightVec, L.dir.xyz), 0.0f), L.spotPowerRange.x);
	
	// Scale color by spotlight factor.
	return litColor*s;
}