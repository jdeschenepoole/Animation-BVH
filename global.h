#ifndef _global_HG_
#define _global_HG_

#include <string>
#include <vector>		// Add this...
#include <windows.h>
#include <d3d10.h>
#include <d3dx10.h>

#include "VariousTypes.h"

// Add the HR timer...
#include "CHRTimer.h"

extern CHRTimer g_TheTimer;

#include "CObjectManager.h"
#include "CBiped.h"			// ****

#include "CCamera.h"

#define PI 3.14159f

extern CObjectManager g_theObjectManager;

extern HINSTANCE   g_hInst;	// = NULL;
extern HWND        g_hWnd;	// = NULL;
// DX variables...
extern ID3D10Device*           g_pd3dDevice;		// = NULL;
extern IDXGISwapChain*         g_pSwapChain;		// = NULL;
extern ID3D10RenderTargetView* g_pRenderTargetView;	// = NULL;

extern UINT g_windowWidth;
extern UINT g_windowHeight;

template <class T>
T getRand(T lowRange, T highRange)
{
	if ( lowRange > highRange )
	{
		T temp = lowRange;
		lowRange = highRange;
		highRange = temp;
	}
	T delta = highRange - lowRange;
	delta = ( static_cast<T>( rand() ) / static_cast<T>(RAND_MAX) ) * delta;
	return delta + lowRange;
}

// **********************************************
extern ID3D10Effect*               g_pEffect;				// = NULL;
extern ID3D10EffectTechnique*      g_pTechnique;			// = NULL;
extern ID3D10InputLayout*          g_pVertexLayout;		// = NULL;
extern ID3D10Buffer*               g_pVertexBuffer;		// = NULL;
extern ID3D10Buffer*               g_pIndexBuffer;			// = NULL;
extern ID3D10EffectMatrixVariable* g_pWorldVariable;		// = NULL;
extern ID3D10EffectMatrixVariable* g_pViewVariable;		// = NULL;
extern ID3D10EffectMatrixVariable* g_pProjectionVariable;	// = NULL;
extern D3DXMATRIX                  g_World;
extern D3DXMATRIX                  g_View;
extern D3DXMATRIX                  g_Projection;
// **********************************************
extern ID3D10Texture2D*			g_pDepthStencilBuffer;
extern ID3D10DepthStencilView*	g_pDepthStencilView;
// **********************************************

bool InitDirectXAndCreateWindow( HINSTANCE hInstance, int nCmdShow, 
	                             std::wstring title, 
								 UINT width, UINT height, 
								 std::wstring &error );
LRESULT CALLBACK    WndProc( HWND, UINT, WPARAM, LPARAM );

void SimulationStep();	// Added Week 3, day 1
void Render();

// **********************************************
bool LoadAndPrepareShaderEffectFile( std::wstring shaderFileName, std::string effectName, std::wstring &error );
// **********************************************


// Lighting and material stuff
//extern ID3D10EffectVectorVariable* g_pBunnyColourVariable;// = NULL;

extern LightDesc g_theLight;	// NOTE: we will likely want more than one of these.
extern CCamera g_theCamera;
extern int g_lightType;			// eventually, this is the kind of light we want


extern ID3D10EffectVectorVariable*	g_pEyeVariable;			// "eye" in shader: Eye coordinates
extern ID3D10EffectVariable*		g_pLightVariable;		// "light" in shader: Description of our light
extern ID3D10EffectScalarVariable*	g_pLightTypeVariable;	// Type of light
extern ID3D10EffectVariable*		g_pObjectMaterialVariable;	// Description of the material of our object



// Will contain all our objects...
extern std::vector<CObject> g_vecObjects;	

extern std::vector< CBiped* > g_vec_pBipeds;		// Contains the BVH models
extern std::vector< CBiped* > g_vec_pAltBipeds;
extern int currentBiped;
#endif 