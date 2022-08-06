// SimulationStep.cpp
// This will update the world (i.e. the objects)
// Note that this is *separate* from the 
//    Render() function....

#include "global.h"
#include "globalAngleAdjust.h"	// Bullet time...
#include <sstream> // For changing the window title text...

void UpdateObject( std::vector<CObject>::iterator itObject, float deltaSeconds );

void SimulationStep()
{
	CHRTimer simTimer;
	simTimer.Reset();
	simTimer.Start();


	// Determine how many seconds have gone by
	//	since the last call to this function...
	::g_TheTimer.Stop();

	float deltaSeconds = ::g_TheTimer.GetElapsedSeconds();

	const float MIN_TIMESTEP = 0.01f;
	if ( deltaSeconds > MIN_TIMESTEP )
	{
		deltaSeconds = MIN_TIMESTEP;
	}

	::g_TheTimer.Reset();
	::g_TheTimer.Start();

	// rotated all the objects based on the rotational speed.
	std::vector<CObject>::iterator itObjects;
	for ( itObjects = g_vecObjects.begin(); 
		  itObjects != g_vecObjects.end(); itObjects++ )
	{
		UpdateObject( itObjects, deltaSeconds );
		// Now the children...
		std::vector< CObject >::iterator itChildObjects;
		for ( itChildObjects = itObjects->vecChildObjects.begin(); 
			  itChildObjects != itObjects->vecChildObjects.end(); itChildObjects++ )
		{
			UpdateObject( itChildObjects, deltaSeconds );
		}
	}

	// Update the bipeds...
	std::vector< CBiped* >::iterator itBipeds;
	for ( itBipeds = g_vec_pBipeds.begin(); itBipeds != g_vec_pBipeds.end(); itBipeds++ )
	{
		// Get the root node for this biped...
		// Determine how many frames we need to move based on delta time.
		float secondsToUpdate = deltaSeconds / ::BulletTimeAdjustBaby;
		if ( !(*itBipeds)->IncrementFrame( secondsToUpdate, true ) )
		{	// Couldn't update the frame for some reason
			MessageBox( NULL, L"Error: (*itBipeds)->IncrementFrame() failed.", L"Sorry it didn't work out.", MB_OK );
		}
	}

	//Checkpoint3.3
	//std::vector< CBiped* >::iterator itAltBipeds;
	for ( itBipeds = g_vec_pAltBipeds.begin(); itBipeds != g_vec_pAltBipeds.end(); itBipeds++ )
	{
		// Get the root node for this biped...
		// Determine how many frames we need to move based on delta time.
		float secondsToUpdate = deltaSeconds / ::BulletTimeAdjustBaby;
		if ( !(*itBipeds)->IncrementFrame( secondsToUpdate, true ) )
		{	// Couldn't update the frame for some reason
			MessageBox( NULL, L"Error: (*itBipeds)->IncrementFrame() failed.", L"Sorry it didn't work out.", MB_OK );
		}
	}

	// End
	simTimer.Stop();
	float timeSpend = simTimer.GetElapsedSeconds();
	int ImASexyBreakPoint = 0;
}

// This is added, too, to update the child objects...
void UpdateObject( std::vector<CObject>::iterator itObject, float deltaSeconds )
{
	itObject->preRotationXYZ.x += ( itObject->preRotationSpeedXYZ.x * deltaSeconds );
	itObject->preRotationXYZ.y += ( itObject->preRotationSpeedXYZ.y * deltaSeconds );
	itObject->preRotationXYZ.z += ( itObject->preRotationSpeedXYZ.z * deltaSeconds );

	itObject->postRotationXYZ.x += ( itObject->postRotationSpeedXYZ.x * deltaSeconds );
	itObject->postRotationXYZ.y += ( itObject->postRotationSpeedXYZ.y * deltaSeconds );
	itObject->postRotationXYZ.z += ( itObject->postRotationSpeedXYZ.z * deltaSeconds );
}