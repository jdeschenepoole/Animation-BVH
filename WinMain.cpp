#include "global.h"

MaterialInfo getRandomMaterial(void)
{
	MaterialInfo randMaterial;
	// Place 1.0f in all the 4th locations (so math will work or ambient will be 'off')
	randMaterial.ambient.w = 1.0f;
	randMaterial.diffuse.w = 1.0f;
	randMaterial.spec.w = 1.0f;

	randMaterial.ambient.x = getRand<float>(0.0f, 1.0f);
	randMaterial.ambient.y = getRand<float>(0.0f, 1.0f);
	randMaterial.ambient.z = getRand<float>(0.0f, 1.0f);

	randMaterial.diffuse.x = getRand<float>(0.0f, 1.0f);
	randMaterial.diffuse.y = getRand<float>(0.0f, 1.0f);
	randMaterial.diffuse.z = getRand<float>(0.0f, 1.0f);

	randMaterial.spec.x = getRand<float>(0.0f, 1.0f);
	randMaterial.spec.y = getRand<float>(0.0f, 1.0f);
	randMaterial.spec.z = getRand<float>(0.0f, 1.0f);

	return randMaterial;
}

void CreateABunnySolarSystemBaby( CPlyInfo plyInfo );

int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
 
	// Save the "handle to the instance." a.k.a. the number that windows uses to identify this application instance
	::g_hInst = hInstance;

	std::wstring error;
	if ( !InitDirectXAndCreateWindow( hInstance, nCmdShow, L"My First DirectX", 1024, 768, error ) )
	{
		MessageBox( NULL, error.c_str(), L"Something is Wrong", MB_OK );
		exit( -1 );
	}

	if ( !LoadAndPrepareShaderEffectFile( L"Lights05.fx", "Render", error ) )
	{
		MessageBox( NULL, error.c_str(), L"Something is wrong", MB_OK );
		exit( -1 );
	}

	// #Checkpoint3 - Below
	// Create a Biped
	CBiped* p_tempBiped = new CBiped();
	
	// Create the map that links the BVH nodes to the ply information...

	if ( !p_tempBiped->CreateBiped( L"ObamaBones229\\Zombie.bvh", L"BVH_PLY_INFO.txt", error ) )
	//if ( !p_tempBiped->CreateBiped( L"ObamaBones229\\FlipBAck.bvh", L"BVH_PLY_INFO.txt", error ) )
	{
		MessageBox( NULL, error.c_str(), L"Something is wrong.", MB_OK );
		return -1;
	}
	// The biped thingy loaded and contains the BVH items details, including the ply files.
	// So load the ply files...
	
	error = L"";
	if ( !g_theObjectManager.LoadPlyModelTypes( p_tempBiped->PlyFilesWeAreUsing, 
		                                        ::g_pd3dDevice,
		                                        ::g_pVertexBuffer, ::g_pIndexBuffer, 
												::g_pVertexLayout, error ) )
	{
		MessageBox( NULL, error.c_str(), L"Can't load ply models", MB_OK );
	}

	g_vec_pBipeds.push_back( p_tempBiped );

		// Create a Biped
	CBiped* p_tempBiped2 = new CBiped();
	
	// Create the map that links the BVH nodes to the ply information...

	if ( !p_tempBiped2->CreateBiped( L"ObamaBones229\\Idle.bvh", L"BVH_PLY_INFO.txt", error ) )
	//if ( !p_tempBiped->CreateBiped( L"ObamaBones229\\FlipBAck.bvh", L"BVH_PLY_INFO.txt", error ) )
	{
		MessageBox( NULL, error.c_str(), L"Something is wrong.", MB_OK );
		return -1;
	}

	p_tempBiped2->moveBiped(10.0f, 0.0f, 20.0f);

	g_vec_pBipeds.push_back( p_tempBiped2 ); 

	// Alt Bipeds

	CBiped* p_tempAltBiped = new CBiped();
	
	// Create the map that links the BVH nodes to the ply information...

	if ( !p_tempAltBiped->CreateBiped( L"ObamaBones229\\Idle.bvh", L"BVH_PLY_INFO.txt", error ) )
	//if ( !p_tempAltBiped->CreateBiped( L"ObamaBones229\\FlipBAck.bvh", L"BVH_PLY_INFO.txt", error ) )
	{
		MessageBox( NULL, error.c_str(), L"Something is wrong.", MB_OK );
		return -1;
	}
	// The biped thingy loaded and contains the BVH items details, including the ply files.
	// So load the ply files...
	
	error = L"";

	g_vec_pAltBipeds.push_back( p_tempAltBiped );

		// Create a Biped
	CBiped* p_tempAltBiped2 = new CBiped();
	
	// Create the map that links the BVH nodes to the ply information...

	if ( !p_tempAltBiped2->CreateBiped( L"ObamaBones229\\Zombie.bvh", L"BVH_PLY_INFO.txt", error ) )
	//if ( !p_tempAltBiped->CreateBiped( L"ObamaBones229\\FlipBAck.bvh", L"BVH_PLY_INFO.txt", error ) )
	{
		MessageBox( NULL, error.c_str(), L"Something is wrong.", MB_OK );
		return -1;
	}

	p_tempAltBiped2->moveBiped(10.0f, 0.0f, 20.0f);

	g_vec_pAltBipeds.push_back( p_tempAltBiped2 );

	// #CheckPoint3 - Above

	float gridLimit = 100.0f;
	float gridStep = 4.0f;
	for ( float x = -gridLimit; x <= (gridLimit + gridStep); x += gridStep )
	{
		for ( float z = -gridLimit; z <= (gridLimit + gridStep); z += gridStep )
		{
			CObject myBunny;
			//CPlyInfo plyInfo = ::g_theObjectManager.GetRandomModel();
			CPlyInfo plyInfo;
			g_theObjectManager.GetPlyRenderingInfoByPlyFileName( L"BVH_PLY\\FlatBox.ply", plyInfo, error );
			myBunny.plyFileName = plyInfo.plyFileName;

			myBunny.numTriangles = plyInfo.numberOfElements;
			myBunny.numVertices = plyInfo.numberOfVertices;
			myBunny.maxExtent = plyInfo.extent;
			myBunny.Scale = 1.0f / myBunny.maxExtent;		// Scale object to 3.0f

			// Set the material for the bunny...
			myBunny.material = getRandomMaterial();

			// Place the bunny
			myBunny.translationXYZ.x = x;
			myBunny.translationXYZ.y = -15.0f;
			myBunny.translationXYZ.z = z;
			myBunny.preRotationXYZ.y = +PI/2.0f;
			
			//myBunny.postRotationSpeedXYZ.y = -PI / 2.0f;

			//myBunny.preRotationSpeedXYZ.y = getRand<float>( 0.01f, 1.0f );

			g_vecObjects.push_back( myBunny );
		}
	}


	// Place the camera (aka the 'eye')
	::g_theCamera.SetEyeXYZ( 0.0f, 5.0f, -20.0f );
	::g_theCamera.SetTargetXYZ( 0.0f, 0.0f, 0.0f );
	::g_theCamera.SetUpXYZ( 0.0f, 1.0f, 0.0f );

	// ****************************************************************************
	// Set up the light...
	g_theLight.material.ambient = D3DXVECTOR4( 0.3f, 0.3f, 0.3f, 1.0f );	// white, but dim
	g_theLight.material.diffuse = D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f );	// All 1.0f = white
	g_theLight.material.spec = D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f );		// White highlight

	g_theLight.dir = ::D3DXVECTOR4( -15.0f, +15.0f, 20.0f, 1.0f );	// Point at origin (if directional or spot)
	g_theLight.attenuation = D3DXVECTOR4( 0.005f, 0.005f, 0.005f, 1.0f );
	g_theLight.pos = D3DXVECTOR4( -5.0f, 5.0f, -20.0f, 1.0f );		// XYZ = -3, 10, 16
																	// To the left (-3), 
																	// above (10), 16 units in the distance
	g_theLight.spotPowerRange.x = 100.0f;	// Spot power (if it's a spot light)
	g_theLight.spotPowerRange.y = 50.0f;		// Range of spot and point light
	g_theLight.spotPowerRange.z = 0.0f;		// Not used
	g_theLight.spotPowerRange.w = 1.0f;		// Not used

	::g_lightType = 1;

	// ****************************************************************************

	// Start the timer...
	::g_TheTimer.Start();
	
	// Main message loop
    MSG msg = {0};
    while( WM_QUIT != msg.message )
    {
        if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        else
        {
			// Update simulation...
			SimulationStep();

            Render();
        }
    }

	// Shut'er down!

	if ( !g_vec_pBipeds.empty() )
	{
		std::vector< CBiped* >::iterator it_pBiped;
		for ( it_pBiped = g_vec_pBipeds.begin(); it_pBiped != g_vec_pBipeds.end(); it_pBiped++ )
		{
			delete (*it_pBiped );
		}
	}

	if( g_pd3dDevice ) g_pd3dDevice->ClearState();

    if( g_pRenderTargetView ) g_pRenderTargetView->Release();
    if( g_pSwapChain ) g_pSwapChain->Release();
    if( g_pd3dDevice ) g_pd3dDevice->Release();

    return ( int )msg.wParam;
}

void SetPlyMaterialScale( CObject& theObject, CPlyInfo plyInfo )
{
	theObject.numTriangles = plyInfo.numberOfElements;
	theObject.numVertices = plyInfo.numberOfVertices;
	theObject.maxExtent = plyInfo.extent;
	theObject.material = getRandomMaterial();
}

//void CreateABunnySolarSystemBaby( CPlyInfo plyInfo )
//{
//	// To simplify the look of the code here, I've added a function that 
//	//	sets the ply rendering information in a single call, rather than
//	//	repeating it over and over again (i.e. they are all bunnies...)
//
//	CObject sunBunny;
//	sunBunny.name = L"Sun";
//	SetPlyMaterialScale( sunBunny, plyInfo );
//	sunBunny.bIsRootNodeSetToOrigin = true;
//
//	// Now the earth
//	CObject Earth;
//	Earth.name = L"Earth";
//	SetPlyMaterialScale( Earth, plyInfo );
//	Earth.material = getRandomMaterial();
//	Earth.bIsRootNodeSetToOrigin = false;
//	Earth.translationXYZ.x = 2.0f;	// 2.0 units away from the sun (ties to scale of THIS object)
//	Earth.preRotationSpeedXYZ.y = 5.0f;		// This is the "spin" rotation
//	Earth.postRotationSpeedXYZ.y = 0.5f;	// This is the "orbit" rotation 
//	Earth.postRotationXYZ.x = -0.6f;
//
//	// Now the moon
//	CObject Moon;
//	Moon.name = L"Moon";
//	SetPlyMaterialScale( Moon, plyInfo );
//	Moon.bIsRootNodeSetToOrigin = false;
//	Moon.translationXYZ.x = 0.5f;	
//	Moon.preRotationSpeedXYZ.y = 10.0f;
//	Moon.postRotationSpeedXYZ.y = 1.0f;
//	Moon.postRotationXYZ.x = +0.2f;
//
//	// Add the Moon to the Earth...
//	Earth.vecChildObjects.push_back( Moon );
//
//	// Add the Earth to the Sun...
//	sunBunny.vecChildObjects.push_back( Earth );
//
//	// Now add the sun bunny to the list of objects...
//	::g_vecObjects.push_back( sunBunny );
//}

