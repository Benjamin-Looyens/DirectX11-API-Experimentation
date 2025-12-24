#pragma once

// SDL Headers
#include "SDL.h"
#include "SDL_syswm.h"
#include "SDL_surface.h"
#include "SDL_image.h"

// Framework Headers
#include "Timer.h"
#include "DataTypes.h"

// Forward Declarations
class Effect;
class SolidEffect;
class AlphaEffect;

struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain;
struct ID3D11Texture2D;
struct ID3D11DepthStencilView;
struct ID3D11Resource;
struct ID3D11RenderTargetView;
struct ID3D11RasterizerState;

namespace dae {
	class Mesh;
	struct Camera;
	struct RenderSettings;
}

namespace dae
{
	class DXRenderer final
	{
	public:
		DXRenderer(SDL_Window* pWindow);
		~DXRenderer();

		DXRenderer(const DXRenderer&) = delete;
		DXRenderer(DXRenderer&&) noexcept = delete;
		DXRenderer& operator=(const DXRenderer&) = delete;
		DXRenderer& operator=(DXRenderer&&) noexcept = delete;

		void Update(const Timer* pTimer, dae::Camera& cam, std::vector<Mesh>& solidMeshes, std::vector<Mesh>& alphaMeshes, RenderSettings& settings);
		void Render(const std::vector<Mesh>& solidMeshes, const std::vector<Mesh>& alphaMeshes, const RenderSettings& settings) const;

		ID3D11Device* GetDevice() const { return m_pDevice; };

	private:
		SDL_Window* m_pWindow{};

		int m_Width{};
		int m_Height{};

		bool m_IsInitialized{ false };
		
		ColorRGB m_BackgroundColor{ 0.39f, 0.59f, 0.93f };

		// DirectX
		HRESULT InitializeDirectX();

		ID3D11Device* m_pDevice{};
		ID3D11DeviceContext* m_pDeviceContext{};
		
		IDXGISwapChain* m_pSwapChain{};

		ID3D11Texture2D* m_pDepthStencilBuffer{};
		ID3D11DepthStencilView* m_pDepthStencilView{};

		ID3D11Resource* m_pRenderTargetBuffer{};
		ID3D11RenderTargetView* m_pRenderTargetView{};

		ID3D11RasterizerState* m_pNoCull{};
		ID3D11RasterizerState* m_pFrontCull{};
		ID3D11RasterizerState* m_pBackCull{};
	};
}
