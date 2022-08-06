// mfeeney@fanshawec.ca
// The lighting functions are (modified) from Frank Luna's DirectX 10 book (chapter 6)

#include "HLSL_4_BasicLightFunctions.fx"

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
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
	float4 VertexPos : POSITION;
	float4 VertexNorm : NORMAL;
};

struct VS_OUTPUT
{
    float4 VertexPosMVP : SV_POSITION;		// NOTE: These are the final vertex location (after world*view*projection)
	float4 VertexPosWorld : POSITION;
    float4 VertexNormalWorld : NORMAL;		//       i.e. Model*View*Projection coordinates
};



//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
//VS_OUTPUT VS( float4 Pos : POSITION, float4 Color : COLOR )
VS_OUTPUT VS( VS_INPUT input )
{
	
	VS_OUTPUT output = (VS_OUTPUT)0;

	// Combine the matrices first...
	matrix matFinalMVP = mul( World, View );
	matFinalMVP = mul( matFinalMVP, Projection );

	output.VertexPosMVP = input.VertexPos;
	// To place the vertex in the correct location on screen:
	output.VertexPosMVP = mul( input.VertexPos, matFinalMVP );

	// Passed to the pixel shader for correct lighting:
	output.VertexPosWorld = mul( input.VertexPos, World );
	output.VertexNormalWorld = mul( input.VertexNorm, World );

    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( VS_OUTPUT input ) : SV_Target
{
	float4 pixelColour = float4( 0.0f, 0.0f, 0.0f, 1.0f );

	if ( lightType == 0 ) // Parallel light
	{
		pixelColour = ParallelLight( objectMaterial, light, 
		                             input.VertexPosWorld, 
		                             input.VertexNormalWorld, eye );	
	}
	else if ( lightType == 1 ) // Parallel light
	{
		pixelColour = PointLight(objectMaterial, light, 
		                         input.VertexPosWorld, 
		                         input.VertexNormalWorld, eye );
	}
	else 
	{
		pixelColour = Spotlight( objectMaterial, light, 
		                         input.VertexPosWorld, 
		                         input.VertexNormalWorld, eye );
	}
	
	return pixelColour;
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
