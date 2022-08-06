#include "global.h"

bool LoadAndPrepareShaderEffectFile( std::wstring shaderFileName, std::string effectName, std::wstring &error )
{
	error = L"OK";
	HRESULT hr = S_OK;

    // Create the effect
    DWORD dwShaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
    // Set the D3D10_SHADER_DEBUG flag if in debug mode
    dwShaderFlags |= D3D10_SHADER_DEBUG;
    #endif
	ID3D10Blob* compilationError = 0;
    hr = D3DX10CreateEffectFromFile( shaderFileName.c_str(), 
		                             NULL, 
									 NULL, 
									 "fx_4_0", 
									 dwShaderFlags, 
									 0, 
									 g_pd3dDevice, 
									 NULL,
									 NULL, 
									 &g_pEffect, 
									 &compilationError, 
									 NULL );
    if( FAILED( hr ) )
    {
		MessageBoxA(0, (char*)compilationError->GetBufferPointer(), 0, 0 );
		// Copy ASCII text to Unicode return string
		//std::string ASCIIError( (char*)compilationError->GetBufferPointer(), 0, 0 );
		return false;   
	}

    // Obtain the technique
    g_pTechnique = g_pEffect->GetTechniqueByName( effectName.c_str() );

    // Obtain the variables
    g_pWorldVariable = g_pEffect->GetVariableByName( "World" )->AsMatrix();
    g_pViewVariable = g_pEffect->GetVariableByName( "View" )->AsMatrix();
    g_pProjectionVariable = g_pEffect->GetVariableByName( "Projection" )->AsMatrix();

	// ***********************************************************************************************
	// ***********************************************************************************************
	
	// Eye coordinates (ID3D10EffectVectorVariable*)
	g_pEyeVariable = g_pEffect->GetVariableByName( "eyeCoords" )->AsVector();	// "eye" in shader
	
	// Description of our light	(ID3D10EffectVariable*)	
	g_pLightVariable = g_pEffect->GetVariableByName( "light" );			// "light" in shader
	
	// Type of light (ID3D10EffectScalarVariable*)
	g_pLightTypeVariable = g_pEffect->GetVariableByName( "lightType" )->AsScalar();	// "lightType" in shader	

	// Description of the material of our object (ID3D10EffectVariable*)	
	g_pObjectMaterialVariable = g_pEffect->GetVariableByName( "objectMaterial" );	// "objectMaterial" in shader
	// ***********************************************************************************************
	// ***********************************************************************************************


	// Prepare the effect...
	// Define the input layout
	D3D10_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 32, D3D10_INPUT_PER_VERTEX_DATA, 0 }
    };
    UINT numElements = sizeof( layout ) / sizeof( layout[0] );

    // Create the input layout
    D3D10_PASS_DESC PassDesc;
    g_pTechnique->GetPassByIndex( 0 )->GetDesc( &PassDesc );
    hr = g_pd3dDevice->CreateInputLayout( layout, numElements, PassDesc.pIAInputSignature,
                                          PassDesc.IAInputSignatureSize, &g_pVertexLayout );
    if( FAILED( hr ) )
	{
		error = L"ERORR: Unable to set the shader pass layout";
		return false;
	}

	return true;
}
