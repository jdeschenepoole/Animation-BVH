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

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
// 
// These are the variables, like the bunny colour, that we change per frame 
//	or per object while we are rendering. 
//--------------------------------------------------------------------------------------
cbuffer cbPerFrame
{
	matrix View;
	matrix Projection;
	float4 eye;
	LightDesc light;
	int lightType;
};

cbuffer cbPerObject
{
	matrix World;
	MaterialInfo objectMaterial;
};

//--------------------------------------------------------------------------------------
struct VS_INPUT
{
	float4 Pos : POSITION;
	float4 Norm : NORMAL;
};

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float4 Norm : NORMAL;			// CHANGED from colour
};

// LIGHTING functions - this is a point light calculation function
//	Takes the light information (and colour, etc.), the material (i.e. colour) 
//	of the object, and the position of the camera (the 'eye').
//	They, with a sprinkly of fairy dust (well, really a bit of fancy math), 
//	calculates the RGB colour the particular pixel or vertex should be. 
// 
//float3 PointLight(SurfaceInfo v, Light L, float3 eyePos)
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

	//if( d > L.spotPowerRange.y )			// y element is range of spot or point light
	//{
		//return float4(0.0f, 0.0f, 0.0f, 1.0f);		// If it's too far, set colour to black.
		//// Note: since it's black, we don't need to to any more calculations.
	//}

	// Normalize the light vector.
	lightVec /= d; 

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
	//litColor =  litColor / dot( L.attenuation, float3(1.0f, d, d*d));

	return float4( litColor.xyz, 1.0f );
}


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
//VS_OUTPUT VS( float4 Pos : POSITION, float4 Color : COLOR )
VS_OUTPUT VS( VS_INPUT input )
{
    //input.x = eye.x;
	
	VS_OUTPUT output = (VS_OUTPUT)0;

	// Combine the matrices first...
	matrix matFinal = mul( World, View );
	matFinal = mul( matFinal, Projection );

	output.Pos = input.Pos;
	output.Pos = mul( output.Pos, matFinal );

	output.Norm = input.Norm;		// CHANGED from colour (pixel shader needs normal)
	//output.Color.xy = eye.xy;
	//output.Color.x = light.dir.x;
	// g_theLight.material.diffuse.x = 1.0f;



    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( VS_OUTPUT input ) : SV_Target
{
	//float4 pixelColor = input.Color;

	// Pass the information to the function and return the colour...
	float4 pixelColor = PointLight(objectMaterial, light, input.Pos, input.Norm, eye );		// ADDED Normal

    return pixelColor;
}


//--------------------------------------------------------------------------------------
technique10 Render
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}



// For reference, this is the original lighting function from Frank Luna's DirectX 10 book:

/*
float3 PointLight(SurfaceInfo v, Light L, float3 eyePos)
{
	float3 litColor = float3(0.0f, 0.0f, 0.0f);
	
	// The vector from the surface to the light.
	float3 lightVec = L.pos - v.pos;
		
	// The distance from surface to light.
	float d = length(lightVec);
	
	if( d > L.range )
		return float3(0.0f, 0.0f, 0.0f);
		
	// Normalize the light vector.
	lightVec /= d; 
	
	// Add the ambient light term.
	litColor += v.diffuse * L.ambient;	
	
	// Add diffuse and specular term, provided the surface is in 
	// the line of site of the light.
	
	float diffuseFactor = dot(lightVec, v.normal);
	[branch]
	if( diffuseFactor > 0.0f )
	{
		float specPower  = max(v.spec.a, 1.0f);
		float3 toEye     = normalize(eyePos - v.pos);
		float3 R         = reflect(-lightVec, v.normal);
		float specFactor = pow(max(dot(R, toEye), 0.0f), specPower);
	
		// diffuse and specular terms
		litColor += diffuseFactor * v.diffuse * L.diffuse;
		litColor += specFactor * v.spec * L.spec;
	}
	
	// attenuate
	return litColor / dot(L.att, float3(1.0f, d, d*d));
}
*/