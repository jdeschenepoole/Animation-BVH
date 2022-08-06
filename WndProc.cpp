#include "global.h"

#include "globalAngleAdjust.h"
#include <sstream> // For changing the window title text...

// Called by the window when soemthing happens (a message is sent)
LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    PAINTSTRUCT ps;
    HDC hdc;

	float cameraMoveSpeed = 1.0f;

    switch( message )
    {
        case WM_PAINT:
            hdc = BeginPaint( hWnd, &ps );
            EndPaint( hWnd, &ps );
            break;

        case WM_DESTROY:
            PostQuitMessage( 0 );
            break;

		// Handle keyboard input...
		case WM_CHAR:
			{	// New scope so we can create variables...
				const float angleAdjustRate = 15.0f;
				const float moveAmount = 5.0f;
				std::wstring error;
				int lastBiped = currentBiped;


				switch (wParam)
				{
				case 'x': case 'X':
					currentBiped++;
					if (currentBiped >= ::g_vec_pBipeds.size())
					{
						currentBiped = 0;
					}
					//::g_vec_pBipeds[currentBiped]->LoadNewBVH(L"BVH_PLY\\Zombie.bvh", error);
					//::g_vec_pBipeds[lastBiped]->LoadNewBVH(L"BVH_PLY\\Idle.bvh", error);
					break;

				case 'q': case 'Q':
					
					//::g_AngleAdjust.x += angleAdjustRate;
					break;

				// #Checkpoint3.1
				case 'a': case 'A':
					::g_vec_pBipeds[currentBiped]->moveBiped(moveAmount * -1.0f, 0.0f, 0.0f);
					::g_vec_pAltBipeds[currentBiped]->moveBiped(moveAmount * -1.0f, 0.0f, 0.0f);
					//::g_AngleAdjust.x -= angleAdjustRate;
					break;
				case 'w': case 'W':
					::g_vec_pBipeds[currentBiped]->moveBiped(0.0f, 0.0f, moveAmount);
					::g_vec_pAltBipeds[currentBiped]->moveBiped(0.0f, 0.0f, moveAmount);
					//::g_AngleAdjust.y += angleAdjustRate;
					break;
				case 's': case 'S':
					::g_vec_pBipeds[currentBiped]->moveBiped(0.0f, 0.0f, moveAmount * -1.0f);
					::g_vec_pAltBipeds[currentBiped]->moveBiped(0.0f, 0.0f, moveAmount * -1.0f);
					//::g_AngleAdjust.y -= angleAdjustRate;
					break;

				case 'd': case 'D':
					::g_vec_pBipeds[currentBiped]->moveBiped(moveAmount, 0.0f, 0.0f);
					::g_vec_pAltBipeds[currentBiped]->moveBiped(moveAmount, 0.0f, 0.0f);
					//::g_AngleAdjust.z -= angleAdjustRate;
					break;
				case 'p': case 'P':
					::g_AngleAdjust.x = ::g_AngleAdjust.y = ::g_AngleAdjust.z = 0.0f;
					break;
				case 'e': case 'E':
					//::g_AngleAdjust.z += angleAdjustRate;
					break;
				// Bullet time:
				case 'n': case 'N':
					::BulletTimeAdjustBaby += 0.1f;
					break;
				case 'm': case 'M':
					::BulletTimeAdjustBaby -= 0.1f;
					// catch so it does not go below 0
					if (::BulletTimeAdjustBaby < 0.0f)
					{
						::BulletTimeAdjustBaby = 0.0f;
					}
					break;
				case 'B': case 'b':
					if ( ::reverseAnimation == false)
						::reverseAnimation = true;
					else
						::reverseAnimation = false;
					break;

				
				}
				// Now change the window title text...
				std::wstringstream ss;
				ss.setf( std::ios_base::fixed );
				ss.precision( 2 );
				ss << "x,y,z : " << ::g_AngleAdjust.x << ", " << ::g_AngleAdjust.y << ", " << ::g_AngleAdjust.z
					<< " bullet time: " << BulletTimeAdjustBaby << " CurrentFrame: " << ::g_vec_pBipeds[0]->getCurrentFrame();
				SetWindowText( hWnd, ss.str().c_str() );
			}
			break;
		case WM_KEYDOWN: 
            switch (wParam) 
            { 
                case VK_LEFT: 
					// Process the LEFT ARROW key. 
					::g_theCamera.eyeXYZ.x -= cameraMoveSpeed;
                    break; 
				case VK_RIGHT:
					::g_theCamera.eyeXYZ.x += cameraMoveSpeed;
					break;
				// Added this, too
				case VK_UP:
					::g_theCamera.eyeXYZ.y += cameraMoveSpeed;
					break;
				case VK_DOWN:
					::g_theCamera.eyeXYZ.y -= cameraMoveSpeed;
					break;
			}

        default:
            return DefWindowProc( hWnd, message, wParam, lParam );
    }

    return 0;
}

