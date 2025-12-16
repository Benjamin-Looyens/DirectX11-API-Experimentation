// Behavior Defines
#define OUTPUT_DEVICE_DETAILS

//External includes
#include "SDL.h"
#include "SDL_surface.h"

// Standard includes
#include <iostream>

//Project includes
#include "Renderer.h"
#include "DataTypes.h"
#include "Effect.h"
#include "Mesh.h"
#include "Utils.h"
#include "Texture.h"

using namespace dae;

Renderer::Renderer(SDL_Window* pWindow) :
	m_pWindow(pWindow)
{
	//Initialize
	SDL_GetWindowSize(pWindow, &m_Width, &m_Height);

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

	// Camera Initialization
	m_Camera.Initialize(m_Width, m_Height, 45.0f, { 0.0f, 0.0f, -50.0f });

	// Vehicle Texture Initilization
	std::wstring vehicleEffectPath{ L"resources/PosCol3D.fx" };
	std::string vehicleDiffusePath{ "resources/vehicle_diffuse.png" };
	std::string vehicleNormalPath{ "resources/vehicle_normal.png" };
	std::string vehicleSpecularPath{ "resources/vehicle_specular.png" };
	std::string vehicleGlossPath{ "resources/vehicle_gloss.png" };

	m_pVehicleEffect = new SolidEffect(m_pDevice, vehicleEffectPath);

	dae::Texture* pDiffuse =	dae::Texture::LoadFromFile(m_pDevice, vehicleDiffusePath);
	dae::Texture* pNormal =		dae::Texture::LoadFromFile(m_pDevice, vehicleNormalPath);
	dae::Texture* pSpecular =	dae::Texture::LoadFromFile(m_pDevice, vehicleSpecularPath);
	dae::Texture* pGloss =		dae::Texture::LoadFromFile(m_pDevice, vehicleGlossPath);

	m_pVehicleEffect->SetDiffuseMap(pDiffuse);
	m_pVehicleEffect->SetNormalMap(pNormal);
	m_pVehicleEffect->SetSpecularMap(pSpecular);
	m_pVehicleEffect->SetGlossinessMap(pGloss);

	// Mesh Initialization
	std::vector<Vertex> vehicleVerts{};
	std::vector<uint32_t> vehicleIndices{};
	Utils::ParseOBJ("resources/vehicle.obj", vehicleVerts, vehicleIndices);
	m_pVehicle = new Mesh(m_pDevice, m_pVehicleEffect, vehicleVerts, vehicleIndices);

	Utils::ParseOBJ("resources/vehicle.obj", vertices, indices);
	m_pMesh = new Mesh{ m_pDevice, pEffect, vertices, indices };

	// Free Temporary Memory Initializations
	delete pDiffuse;
	delete pNormal;
	delete pSpecular;
	delete pGloss;
}

Renderer::~Renderer()
{
	// Release in reverse order
	if (m_pRenderTargetView) m_pRenderTargetView->Release();
	if (m_pRenderTargetBuffer) m_pRenderTargetBuffer->Release();
	if (m_pDepthStencilView) m_pDepthStencilView->Release();
	if (m_pDepthStencilBuffer) m_pDepthStencilBuffer->Release();
	if (m_pSwapChain) m_pSwapChain->Release();

	if (m_pDeviceContext) {
		m_pDeviceContext->ClearState();
		m_pDeviceContext->Flush();
		m_pDeviceContext->Release();
	}

	if (m_pDevice) m_pDevice->Release();

	delete m_pVehicle;
	delete m_pVehicleEffect;
}

void Renderer::Update(const Timer* pTimer)
{
	if (!m_IsInitialized) return;

	// Update Members
	m_Camera.Update(pTimer);

	if (m_DoRotation) {
		m_RotCounter += std::fmod(pTimer->GetElapsed(), PI_2);
		m_WorldTransform.SetRotationY(m_RotCounter);
	}
}

	// Calculate Current Context Variables
	Matrix world = m_WorldTransform.GetFull(); // HACK : Maybe I could calculate this in the update function, and then store it to be used here?
	Matrix worldViewProj = world * m_Camera.viewMatrix * m_Camera.projectMatrix; // Cant be const due to function later

	// Update DirectX Side
	m_pVehicleEffect->GetWorld()->SetMatrix(reinterpret_cast<float*>(&world));
	m_pVehicleEffect->GetCameraPos()->SetFloatVector(reinterpret_cast<float*>(&m_Camera.origin));
	m_pVehicleEffect->GetWorldViewProj()->SetMatrix(reinterpret_cast<float*>(&worldViewProj));

void Renderer::Render() const
{
	if (!m_IsInitialized) return;

	// Clear Views
	constexpr float color[4] = { 0.0f, 0.0f, 0.3f, 1.0f };
	m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, color);
	m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// Render Solid Meshes
	if (m_pVehicle) m_pVehicle->Render(m_pDeviceContext, m_CurTechnique);

	// Swap Buffers (Present)
	m_pSwapChain->Present(0, 0);
}

void dae::Renderer::CycleFiltering()
{
	// Yuck
	m_CurTechnique = static_cast<FilterTechnique>((static_cast<uint8_t>(m_CurTechnique) + 1) % static_cast<uint8_t>(FilterTechnique::TECHNIQUE_COUNT));

	// Yuck part 2
	std::cout << "Filtering Technique = ";
	switch (m_CurTechnique)
	{
	case FilterTechnique::POINT:
		std::cout << "POINT\n";
		break;
	case FilterTechnique::LINEAR:
		std::cout << "LINEAR\n";
		break;
	case FilterTechnique::ANISOTROPIC:
		std::cout << "ANISOTROPIC\n";
		break;
	}
}

void dae::Renderer::ToggleRotation()
{
	m_DoRotation = !m_DoRotation;

	std::cout << "Mesh Rotation = ";
	switch (m_DoRotation)
	{
	case true:
		std::cout << "ON\n";
		break;
	case false:
		std::cout << "OFF\n";
		break;
	}
}

HRESULT Renderer::InitializeDirectX()
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

	// Release Temporary Pointers
	pFactory->Release();

#ifdef OUTPUT_DEVICE_DETAILS
	pAdapter->Release();
	pDXGIDevice->Release();
#endif

	return S_OK;
}