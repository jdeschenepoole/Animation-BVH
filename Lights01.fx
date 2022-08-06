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
	float4 shoeSize;
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
    float4 Color : COLOR0;
};

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

	output.Color = input.Norm; 
	//output.Color.xy = eye.xy;
	//output.Color.x = light.dir.x;
	// g_theLight.material.diffuse.x = 1.0f;
	//output.Color = objectMaterial.diffuse;


    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( VS_OUTPUT input ) : SV_Target
{
    return input.Color;
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

