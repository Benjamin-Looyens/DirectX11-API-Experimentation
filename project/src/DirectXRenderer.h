#pragma once

// SDL Headers
#include "SDL.h"
#include "SDL_syswm.h"
#include "SDL_surface.h"
#include "SDL_image.h"

// DirectX Headers
#include <dxgi.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3dx11effect.h>

// Framework Headers
#include "Timer.h"
#include "DataTypes.h"

// Forward Declarations
class Mesh;
class Effect;
class SolidEffect;
class AlphaEffect;

namespace dae {
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

		void Update(const Timer* pTimer, dae::Camera& cam, std::vector<Mesh>& solidMeshes, std::vector<Mesh>& alphaMeshes, ::RenderSettings& settings);
		void Render(const std::vector<Mesh>& solidMeshes, const std::vector<Mesh>& alphaMeshes, const ::RenderSettings& settings) const;

		ID3D11Device* GetDevice() const { return m_pDevice; };

	private:
		SDL_Window* m_pWindow{};

		int m_Width{};
		int m_Height{};

		bool m_IsInitialized{ false };

		// DirectX
		HRESULT InitializeDirectX();

		ID3D11Device* m_pDevice{};
		ID3D11DeviceContext* m_pDeviceContext{};
		
		IDXGISwapChain* m_pSwapChain{};

		ID3D11Texture2D* m_pDepthStencilBuffer{};
		ID3D11DepthStencilView* m_pDepthStencilView{};

		ID3D11Resource* m_pRenderTargetBuffer{};
		ID3D11RenderTargetView* m_pRenderTargetView{};
	};
}
