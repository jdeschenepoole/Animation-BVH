#include "global.h"

bool InitDirectXAndCreateWindow( HINSTANCE hInstance, int nCmdShow, 
	                             std::wstring title, 
								 UINT width, UINT height, 
								 std::wstring &error )
{
	error = L"OK";

    // Register class
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof( WNDCLASSEX );
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor( NULL, IDC_ARROW );
    wcex.hbrBackground = ( HBRUSH )( COLOR_WINDOW + 1 );
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = L"INFO5058DXBasics";
    wcex.hIconSm = LoadIcon( wcex.hInstance, ( LPCTSTR )IDI_APPLICATION );
	// Try a "register" this type of window... so that we can create it later
    if( !RegisterClassEx( &wcex ) )
	{
		error = L"ERROR: Could not register window class";
        return false;
	}

    // Create window
    g_hInst = hInstance;
    RECT rc = { 0, 0, width, height };
    AdjustWindowRect( &rc, WS_OVERLAPPEDWINDOW, FALSE );
	g_hWnd = 0;
    g_hWnd = CreateWindow( L"INFO5058DXBasics", title.c_str(), WS_OVERLAPPEDWINDOW,
                           CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance,
                           NULL );

	// Try to create the window...
    if( !g_hWnd )
	{
		error = L"Could not create window.";
		g_hWnd = 0;	// The handle to the window is invalid, so clear it.
        return false;
	}

    ShowWindow( g_hWnd, nCmdShow );

	// Now we init DirectX...
	HRESULT hr = S_OK;;

	// We are getting the size of the window we just created.
    GetClientRect( g_hWnd, &rc );
    g_windowWidth = rc.right - rc.left;
    g_windowHeight = rc.bottom - rc.top;

	// If we are in debug mode (which we likely are), then add the "add debug flag stuff" to the device we are creating
	UINT createDeviceFlags = 0;
#ifdef _DEBUG
    createDeviceFlags |= D3D10_CREATE_DEVICE_DEBUG;
#endif

	// Set up the information to create a hardware device that's windowed, etc.
	DXGI_SWAP_CHAIN_DESC sd;			// Structure that hold the description of the device we're creating
    ZeroMemory( &sd, sizeof( sd ) );	// Clear everything to zeros
    sd.BufferCount = 1;					// Two buffers (front and back - we only draw to the back)
    sd.BufferDesc.Width = g_windowWidth;		// To match the window we created
    sd.BufferDesc.Height = g_windowHeight;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;		// Typical: 8 bits for Red, Green, Blue, and "alpha" (in that order)
    sd.BufferDesc.RefreshRate.Numerator = 60;				// 60 / 1 = 60Hz
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;		// Draw to the screen eventually (we can draw other places, too)
    sd.OutputWindow = g_hWnd;								// Draw to the window that has this handle
    sd.SampleDesc.Count = 1;								// Used for "multi-sampling" - elimination of the "jaggies"
    sd.SampleDesc.Quality = 0;								// (see above)
    sd.Windowed = TRUE;					// Application is windowed, not full-screen

    hr = D3D10CreateDeviceAndSwapChain( NULL,			// NULL = "primary" adapter (if you have multi-monitors or more than one video card)
										D3D10_DRIVER_TYPE_HARDWARE,		// Could be "reference," too, but hardware if full DX10
										NULL, 
										createDeviceFlags,		// We added "debug" to this earlier
                                        D3D10_SDK_VERSION,		// must be this for DX10. Is DX11... for ... well you can guess
										&sd,					// Structure above describing what we are trying to create
										&g_pSwapChain,			// The actual thingy we are going to draw to ('swap chain')
										&g_pd3dDevice );		// The DirectX "device" which is really just a way to "get at" everything
	// See if it worked.    
	if( FAILED( hr ) )
	{
		error = L"ERROR: Could not create the DirectX device or swapchain.";
		return false;
	}


	// Now get access to the "back buffer." 
	// This is treated like a "texture" because is basically is one...
    ID3D10Texture2D* pBackBuffer;
    hr = g_pSwapChain->GetBuffer( 0, __uuidof( ID3D10Texture2D ), ( LPVOID* )&pBackBuffer );
    if( FAILED( hr ) )
	{
		error = L"ERROR: Could not get at the back buffer.";
        return false;
	}

	// Now set this back buffer as the "target" for the "renderer." 
	// You can have DirectX "render" to off-screen textures to do a number of things
	// Here, we are just telling it to "just draw to the screen."
	hr = g_pd3dDevice->CreateRenderTargetView( pBackBuffer, NULL, &g_pRenderTargetView );
	// Get rid of the "ID3D10Texture2D" object because we won't be needing it.
	// NOTE: This is a "COM" object, so we call "Release()" not "delete".
	// NEVER, EVER, EVER call "delete" on a COM object...
    pBackBuffer->Release();
    if( FAILED( hr ) )
	{
		error = L"ERROR: Could not set the render target to the back buffer.";
        return false;
	}

	////... and connect the renderer and the render target. 
	//// (You can change this all the time, even during a render call)
	//g_pd3dDevice->OMSetRenderTargets( 1,							// We only have one
	//								  &g_pRenderTargetView,		// Target to draw to
	//								  NULL );					// Stencil & depth buffer - we will add this tomorrow

    // Setup the viewport (what we are going to see)
	// NOTE: This is a 1.0 x 1.0 window into the "world"
    D3D10_VIEWPORT vp;
    vp.Width = g_windowWidth;
    vp.Height = g_windowHeight;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    g_pd3dDevice->RSSetViewports( 1, &vp );

	// NOW, the depth-stencil buffer...
    D3D10_TEXTURE2D_DESC descDepth;
    descDepth.Width = g_windowWidth;
    descDepth.Height = g_windowHeight;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D32_FLOAT;
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D10_USAGE_DEFAULT;
    descDepth.BindFlags = D3D10_BIND_DEPTH_STENCIL;
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;
    hr = g_pd3dDevice->CreateTexture2D( &descDepth, 
		                                NULL, 
										&g_pDepthStencilBuffer );
	if( FAILED( hr ) )
	{
		error = L"ERROR: Could not create the depth-stencil buffer.";
        return false;
	}
    // Create the depth stencil view
    D3D10_DEPTH_STENCIL_VIEW_DESC descDSV;
    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
    hr = g_pd3dDevice->CreateDepthStencilView( g_pDepthStencilBuffer, 
		                                       &descDSV, 
											   &g_pDepthStencilView );
	if( FAILED( hr ) )
	{
		error = L"ERROR: Could not create view of depth-stencil buffer.";
        return false;
	}	


	g_pd3dDevice->OMSetRenderTargets( 1, 
		                              &g_pRenderTargetView, 
									  g_pDepthStencilView );



	return true;
}
