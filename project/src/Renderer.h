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
#include "Camera.h"
#include "DataTypes.h"

// Forward Declarations
class Mesh;
class Effect;
class SolidEffect;
class AlphaEffect;

namespace dae
{
	class Renderer final
	{
	public:
		Renderer(SDL_Window* pWindow);
		~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) noexcept = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;

		void Update(const Timer* pTimer);
		void Render() const; 

		void CycleFiltering();

		void ToggleRotation();

	private:
		SDL_Window* m_pWindow{};

		int m_Width{};
		int m_Height{};

		bool m_IsInitialized{ false };

		// Meshes
		Mesh* m_pVehicle{};
		SolidEffect* m_pVehicleEffect{};

		Mesh* m_pFire{};
		AlphaEffect* m_pFireEffect{};

		Transform m_WorldTransform{}; // TODO : Store in mesh?

		Camera m_Camera;

		SampleTechnique m_CurSamTech{ SampleTechnique::POINT };

		float m_RotCounter{};

		// Toggles
		bool m_DoRotation{ false };

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
