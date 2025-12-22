// Behavior Defines
#define OUTPUT_DEVICE_DETAILS

//External includes
#include "SDL.h"
#include "SDL_surface.h"

// Standard includes
#include <iostream>

//Project includes
#include "DirectXRenderer.h"
#include "DataTypes.h"
#include "Effect.h"
#include "Mesh.h"
#include "Utils.h"
#include "Texture.h"
#include "Camera.h"

using namespace dae;

DXRenderer::DXRenderer(SDL_Window* pWindow) :
	m_pWindow(pWindow)
{
	//Initialize
	SDL_GetWindowSize(pWindow, &m_Width, &m_Height); // TODO : Prob do this only in the DualRasterizer

	//Initialize DirectX pipeline
	const HRESULT result = InitializeDirectX();
	if (result == S_OK)
	{
		m_IsInitialized = true;
		std::cout << "DirectX is initialized and ready!\n";
	}
	else
	{
		std::cout << "DirectX initialization failed with error [" << result << "]!\n";
	}
}

DXRenderer::~DXRenderer()
{
	// Release in reverse order
	if (m_pNoCull) m_pNoCull->Release();
	if (m_pFrontCull) m_pFrontCull->Release();
	if (m_pBackCull) m_pBackCull->Release();
	if (m_pRenderTargetView) m_pRenderTargetView->Release();
	if (m_pRenderTargetBuffer) m_pRenderTargetBuffer->Release();
	if (m_pDepthStencilView) m_pDepthStencilView->Release();
	if (m_pDepthStencilBuffer) m_pDepthStencilBuffer->Release();
	if (m_pSwapChain) m_pSwapChain->Release();

#if defined(DEBUG) || defined(_DEBUG)
	ID3D11Debug* pDebug = nullptr;
	m_pDevice->QueryInterface(__uuidof(ID3D11Debug), (void**)&pDebug);
	pDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
	pDebug->Release();
#endif

	if (m_pDeviceContext) {
		m_pDeviceContext->ClearState();
		m_pDeviceContext->Flush();
		m_pDeviceContext->Release();
	}

	if (m_pDevice) m_pDevice->Release();
}

void DXRenderer::Update(const Timer* pTimer, Camera& cam, std::vector<Mesh>& solidMeshes, std::vector<Mesh>& alphaMeshes, ::RenderSettings& settings)
{
	if (!m_IsInitialized) return;

	// HACK : Maybe I should just pass the world matrix, and not the Render settings entirely (so that it can be const in here)

	// Calculate Current Context Variables
	Matrix world = settings.transform.GetFull(); // Reference ?
	Matrix worldViewProj = world * cam.viewMatrix * cam.projectMatrix; // Cant be const due to function later

	// Render Solid Meshes
	for (Mesh& mesh : solidMeshes) {
		// Not Ideal, but yep
		SolidEffect* pEffect = dynamic_cast<SolidEffect*>(mesh.GetEffect());

		if (pEffect) {

			pEffect->GetWorld()->SetMatrix(reinterpret_cast<float*>(&world));
			pEffect->GetCameraPos()->SetFloatVector(reinterpret_cast<float*>(&cam.origin));
			pEffect->GetWorldViewProj()->SetMatrix(reinterpret_cast<float*>(&worldViewProj));
		}
	}
	
	// Render Alpha Meshes
	for (Mesh& mesh : alphaMeshes) {
		mesh.GetEffect()->GetWorldViewProj()->SetMatrix(reinterpret_cast<float*>(&worldViewProj));
	}
}

void DXRenderer::Render(const std::vector<Mesh>& solidMeshes, const std::vector<Mesh>& alphaMeshes, const ::RenderSettings& settings) const
{
	if (!m_IsInitialized) return;

	// Clear Views
	float color[4] = { 0.0f, 0.0f, 0.3f, 1.0f };
	if (settings.doUniformBackGround) {
		const dae::ColorRGB& bg = settings.uniformBackGround;
		color[0] = bg.r;
		color[1] = bg.g;
		color[2] = bg.b;
	}
	m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, color);
	m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// Set Solid Raster State
	switch (settings.cullMode) {
	case CullMode::BACK_FACE:
		m_pDeviceContext->RSSetState(m_pBackCull);
		break;
	case CullMode::FRONT_FACE:
		m_pDeviceContext->RSSetState(m_pFrontCull);
		break;
	case CullMode::NONE:
		m_pDeviceContext->RSSetState(m_pNoCull);
		break;
	}
	
	// Render Solid Meshes
	for (const Mesh& mesh : solidMeshes) {
		mesh.Render(m_pDeviceContext);
	}

	// Render Alpha Meshes
	if (settings.doFireRender) {
		for (const Mesh& mesh : alphaMeshes) {
			mesh.Render(m_pDeviceContext);
		}
	}

	// Swap Buffers (Present)
	m_pSwapChain->Present(0, 0);
}

HRESULT DXRenderer::InitializeDirectX()
{
	// NOTE : In theory I could make some stuff here constexpr (but shouldn't matter much)

	// Create Device & DeviceContext
	const D3D_FEATURE_LEVEL featLevel = D3D_FEATURE_LEVEL_11_1;
	uint32_t createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG; // Why bitwise or equals (|=), when we initialize it as a 0, so doing = is just the same thing?
#endif

	HRESULT result = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, createDeviceFlags, &featLevel, 1, D3D11_SDK_VERSION, &m_pDevice, nullptr, &m_pDeviceContext);
	if (FAILED(result)) return result;

#ifdef OUTPUT_DEVICE_DETAILS
	// Console output chosen adapter
	IDXGIDevice* pDXGIDevice{};
	m_pDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&pDXGIDevice);

	IDXGIAdapter* pAdapter{};
	pDXGIDevice->GetAdapter(&pAdapter);

	DXGI_ADAPTER_DESC adapterDesc{};
	pAdapter->GetDesc(&adapterDesc);
	std::wcout << L"Adapter [" << adapterDesc.Description << L"] Chosen\n";
#endif

	// Create DXGI Factory
	IDXGIFactory1* pFactory{};
	result = CreateDXGIFactory1(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&pFactory));
	if (FAILED(result)) return result;

	// Initialize Swapchain Values
	DXGI_SWAP_CHAIN_DESC swapChainDesc{};
	swapChainDesc.BufferDesc.Width = m_Width;
	swapChainDesc.BufferDesc.Height = m_Height;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 1;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 60;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.Windowed = true;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = 0;

	// Get SDL handle
	SDL_SysWMinfo sysInfo{};
	SDL_GetVersion(&sysInfo.version);
	SDL_GetWindowWMInfo(m_pWindow, &sysInfo);
	swapChainDesc.OutputWindow = sysInfo.info.win.window;

	// Create Swap Chain
	result = pFactory->CreateSwapChain(m_pDevice, &swapChainDesc, &m_pSwapChain);
	if (FAILED(result)) return result;

	// Initialize Depth Stencil Values
	D3D11_TEXTURE2D_DESC stencilDesc{};
	stencilDesc.Height = m_Height;
	stencilDesc.Width = m_Width;
	stencilDesc.MipLevels = 1;
	stencilDesc.ArraySize = 1;
	stencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	stencilDesc.SampleDesc.Count = 1;
	stencilDesc.SampleDesc.Quality = 0;
	stencilDesc.Usage = D3D11_USAGE_DEFAULT;
	stencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	stencilDesc.CPUAccessFlags = 0;
	stencilDesc.MiscFlags = 0;

	// Initialize Depth Stencil View Values
	CD3D11_DEPTH_STENCIL_VIEW_DESC stencilViewDesc{};
	stencilViewDesc.Format = stencilDesc.Format;
	stencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	stencilViewDesc.Texture2D.MipSlice = 0;

	// Create Resource & Resource View
	result = m_pDevice->CreateTexture2D(&stencilDesc, nullptr, &m_pDepthStencilBuffer);
	if (FAILED(result)) return result;

	result = m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, &stencilViewDesc, &m_pDepthStencilView);
	if (FAILED(result)) return result;

	// Create Render Target & Render Target View
	result = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&m_pRenderTargetBuffer));
	if (FAILED(result)) return result;

	result = m_pDevice->CreateRenderTargetView(m_pRenderTargetBuffer, nullptr, &m_pRenderTargetView);
	if (FAILED(result)) return result;

	// Bind Views (Render Target & Depth Stencil) as Active Buffers
	m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

	// Initialize Viewport Values
	D3D11_VIEWPORT viewport{};
	viewport.Width = static_cast<float>(m_Width);
	viewport.Height = static_cast<float>(m_Height);
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	m_pDeviceContext->RSSetViewports(1, &viewport);

	// Initialize Rasterizer States
	D3D11_RASTERIZER_DESC rasterDesc{};
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = FALSE;
	rasterDesc.DepthClipEnable = TRUE;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	result = m_pDevice->CreateRasterizerState(&rasterDesc, &m_pBackCull);
	if (FAILED(result)) return result;
	
	rasterDesc.CullMode = D3D11_CULL_FRONT;
	result = m_pDevice->CreateRasterizerState(&rasterDesc, &m_pFrontCull);
	if (FAILED(result)) return result;

	rasterDesc.CullMode = D3D11_CULL_NONE;
	result = m_pDevice->CreateRasterizerState(&rasterDesc, &m_pNoCull);
	if (FAILED(result)) return result;

	// Release Temporary Pointers
	pFactory->Release();

#ifdef OUTPUT_DEVICE_DETAILS
	pAdapter->Release();
	pDXGIDevice->Release();
#endif

	return S_OK;
}