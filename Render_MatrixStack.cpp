#include "global.h"

// This is the NEW renderer that takes into account the
//	matrix information stored in the heirarchical new CObject
//

void DrawObject( D3DXMATRIX &matParentWorld, 
	             std::vector<CObject>::iterator itObjects);


void Render()	// 60Hz because of the SWAPCHAIN setttings...
{
    // Just clear the backbuffer
    float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; //red,green,blue,alpha
    g_pd3dDevice->ClearRenderTargetView( g_pRenderTargetView, ClearColor );


    // Clear the depth buffer to 1.0 (max depth)
    g_pd3dDevice->ClearDepthStencilView( g_pDepthStencilView, D3D10_CLEAR_DEPTH, 1.0f, 0 );


    // Initialize the view matrix
	D3DXMatrixLookAtLH( &g_View, 
		                &(::g_theCamera.eyeXYZ), 
						&(::g_theCamera.targetXYZ), 
						&(::g_theCamera.upXYZ) );

    // Initialize the projection matrix
    D3DXMatrixPerspectiveFovLH( &g_Projection, 
							    static_cast<float>(D3DX_PI) * 0.5f, 
		                        static_cast<float>(g_windowWidth) / static_cast<float>(g_windowHeight), 
								0.1f, 100000.0f );

	// ******************************************************************************
	// ******************************************************************************
	// Here we set all the "per frame" stuff - things that are the same for all
	//	the objects in the scene like lighting, perspective matrix, likely view, etc. 
	//
	//		cbuffer cbPerFrame
	//	{
	//		matrix View;
	//		matrix Projection;
	//		float4 eye;
	//		LightDesc light;
	//		int lightType;
	//	};

	g_pViewVariable->SetMatrix( ( float* )&g_View );				// matrix View
    g_pProjectionVariable->SetMatrix( ( float* )&g_Projection );	// matrix Projection
	g_pEyeVariable->SetFloatVector( (float*) &(::g_theCamera.eyeXYZ) );	 // float4 eye
	
	g_pLightVariable->SetRawValue( &g_theLight, 0, sizeof( LightDesc ) );	// LightDesc light

	g_pLightTypeVariable->SetInt( g_lightType );

	// ******************************************************************************
	// ******************************************************************************



	// Now draw all the objects....
	// NOTE: This draws the objects recursively... 
	std::vector<CObject>::iterator itObjects;
	for ( itObjects = g_vecObjects.begin(); 
		  itObjects != g_vecObjects.end(); itObjects++ )
	{
		D3DXMATRIX matFinalWorld = (itObjects->myWorldMatrix);

		DrawObject( matFinalWorld, itObjects );
	}

	// #Checkpoint3
	// 2 vectors of bipeds were used so i could switch between them based on which biped we choose to control
	// Now we draw all the bipeds....
	
	 if (currentBiped == 1)
	 {
		std::vector< CBiped* >::iterator itAltBipeds;
		for ( itAltBipeds = g_vec_pAltBipeds.begin(); itAltBipeds != g_vec_pAltBipeds.end(); itAltBipeds++ )
		{
				// Get the root node for this biped...
				std::wstring error;

				CObject rootObject;
				(*itAltBipeds)->GetRootNodeObject( rootObject, error );

				D3DXMATRIX matFinalWorld = (rootObject.myWorldMatrix);

				std::vector< CObject > vecHOLYCRAP;
				vecHOLYCRAP.push_back( rootObject );

				DrawObject( matFinalWorld, vecHOLYCRAP.begin() );
		}
	 }
	 else
	 {
		std::vector< CBiped* >::iterator itBipeds;
		for ( itBipeds = g_vec_pBipeds.begin(); itBipeds != g_vec_pBipeds.end(); itBipeds++ )
		{
				// Get the root node for this biped...
				std::wstring error;

				CObject rootObject;
				(*itBipeds)->GetRootNodeObject( rootObject, error );

				D3DXMATRIX matFinalWorld = (rootObject.myWorldMatrix);

				std::vector< CObject > vecHOLYCRAP;
				vecHOLYCRAP.push_back( rootObject );

				DrawObject( matFinalWorld, vecHOLYCRAP.begin() );
		}
	  }

    g_pSwapChain->Present( 0, 0 );
}

// This draws the current object using the current world matrix as a starting point
void DrawObject( D3DXMATRIX &matParentWorld, 
	             std::vector<CObject>::iterator itObjects)
{
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );

	::D3DXMATRIX matTranslate;
	::D3DXMATRIX matPreRotateX, matPreRotateY, matPreRotateZ;
	::D3DXMATRIX matPostRotateX, matPostRotateY, matPostRotateZ;
	::D3DXMATRIX matScale;

	D3DXMatrixIdentity( &matTranslate );	
	D3DXMatrixIdentity( &matPreRotateX );	D3DXMatrixIdentity( &matPreRotateY );	D3DXMatrixIdentity( &matPreRotateZ );
	D3DXMatrixIdentity( &matPostRotateX );	D3DXMatrixIdentity( &matPostRotateY );	D3DXMatrixIdentity( &matPostRotateZ );
	D3DXMatrixIdentity( &matScale );

	// Rotate, then translate, then Rotate...
	::D3DXMatrixRotationX( &matPreRotateX, itObjects->preRotationXYZ.x );
	::D3DXMatrixRotationY( &matPreRotateY, itObjects->preRotationXYZ.y );
	::D3DXMatrixRotationZ( &matPreRotateZ, itObjects->preRotationXYZ.z );

	::D3DXMatrixTranslation( &matTranslate, itObjects->translationXYZ.x, itObjects->translationXYZ.y, itObjects->translationXYZ.z );
	
	::D3DXMatrixRotationX( &matPostRotateX, itObjects->postRotationXYZ.x );
	::D3DXMatrixRotationY( &matPostRotateY, itObjects->postRotationXYZ.y );
	::D3DXMatrixRotationZ( &matPostRotateZ, itObjects->postRotationXYZ.z );

	::D3DXMatrixScaling( &matScale, itObjects->Scale, itObjects->Scale, itObjects->Scale );

	// Now combine the matrices....
	// (Note that we are combining this with the object's world matrix...

	//// 1. Scale (this may have to move, depending on what you want)
	//D3DXMatrixMultiply( &matWorld, &matWorld, &matScale );

	// 1. Pre-rotation...
	D3DXMatrixMultiply( &matWorld, &matWorld, &matPreRotateX );
	D3DXMatrixMultiply( &matWorld, &matWorld, &matPreRotateY );
	D3DXMatrixMultiply( &matWorld, &matWorld, &matPreRotateZ );
	// 2. Translation...
	D3DXMatrixMultiply( &matWorld, &matWorld, &matTranslate );
	// 3. Post-rotation
	D3DXMatrixMultiply( &matWorld, &matWorld, &matPostRotateX );
	D3DXMatrixMultiply( &matWorld, &matWorld, &matPostRotateY );
	D3DXMatrixMultiply( &matWorld, &matWorld, &matPostRotateZ );

	//// *** Scale ***
	//// NOTE that this is applied separately and not stored
	//D3DXMatrixMultiply( &matWorld, &matWorld, &matScale );

	// Is this object being drawn relative to the parent?
	if ( !itObjects->bIsRootNodeSetToOrigin )
	{	//Yup
		D3DXMatrixMultiply( &matWorld, &matWorld, &matParentWorld );
	}

	// Update the object's world matrix...
	itObjects->myWorldMatrix = matWorld;


	// Apply this object matrix to the world transform...
	g_pWorldVariable->SetMatrix( ( float* )&matWorld );

	// Point the renderer to the appropriate vertex 
	//	and index buffer


	UINT stride = sizeof( SimpleVertex );
	UINT offset = 0;
	g_pd3dDevice->IASetVertexBuffers( 0, 1, &g_pVertexBuffer, &stride, &offset );
	g_pd3dDevice->IASetIndexBuffer( g_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0 );

	// Set primitive topology
	g_pd3dDevice->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	// ******************************************************************************
	// ******************************************************************************

	// Copy the material information for each object into the shader...
	MaterialInfo tempMaterial = itObjects->material;
	g_pObjectMaterialVariable->SetRawValue( &tempMaterial, 0, sizeof(MaterialInfo) );
	
	// ******************************************************************************
	// ******************************************************************************

	D3D10_TECHNIQUE_DESC techDesc;
	g_pTechnique->GetDesc( &techDesc );
	for( UINT p = 0; p < techDesc.Passes; ++p )
	{
		g_pTechnique->GetPassByIndex( p )->Apply( 0 );
/*		g_pd3dDevice->DrawIndexed( itObjects->numTriangles * 3, 
								0,
								0 );   */    

		// Get the ply information from the model
		std::wstring error = L"";
		CPlyInfo tempPlyInfo;
		if ( g_theObjectManager.GetPlyRenderingInfoByPlyFileName( itObjects->plyFileName, tempPlyInfo, error ) )
		{
			//g_pd3dDevice->DrawIndexed( itObjects->numTriangles * 3, 
			//							tempPlyInfo.indexBufferStartIndex,
			//							tempPlyInfo.vertexBufferStartIndex );        
			g_pd3dDevice->DrawIndexed( tempPlyInfo.numberOfElements * 3, 
										tempPlyInfo.indexBufferStartIndex,
										tempPlyInfo.vertexBufferStartIndex );        
		}
	}

	// And the recursive part...
	std::vector<CObject>::iterator itChildObjects;
	for ( itChildObjects = itObjects->vecChildObjects.begin(); 
		  itChildObjects != itObjects->vecChildObjects.end(); itChildObjects++ )
	{

		//D3DXMATRIX matFinalWorld = (itChildObjects->myWorldMatrix);

		DrawObject( itObjects->myWorldMatrix, itChildObjects );


	}
}