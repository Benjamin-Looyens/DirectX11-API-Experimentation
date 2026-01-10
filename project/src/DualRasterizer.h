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

// Project Headers
#include "Camera.h"
#include "DataTypes.h"
#include "Mesh.h"

// Forward Declarations
namespace dae {
	class SoftwareRenderer;
	class DXRenderer;
	class Timer;
	class Texture;
	class Effect;
	class SolidEffect;
	class AlphaEffect;

	class DualRasterizer final
	{
	public:
		DualRasterizer(SDL_Window* pWindow);
		~DualRasterizer();

		DualRasterizer(const DualRasterizer&) = delete;
		DualRasterizer(DualRasterizer&&) noexcept = delete;
		DualRasterizer& operator=(const DualRasterizer&) = delete;
		DualRasterizer& operator=(DualRasterizer&&) noexcept = delete;

		void Update(const Timer* pTimer);
		void Render() const;

		// Toggles
		void ToggleRotation();
		void ToggleUniformClear();
		
		void ToggleFireMesh();

		void ToggleNormalMap();
		void ToggleDepthRender();
		void ToggleBoundsRender();

		void ToggleConsoleOut();

		// Cycles
		void CycleCullMode();
		void CycleSampleTechnique();
		void CycleLightingMode();
		void CycleRenderer();

		// Other Options
		void ResetCamera();

	private:
		void InitDirectX();
		void InitSoftware();

		// Renderers
		DXRenderer* m_pDirectX{};
		SoftwareRenderer* m_pSoftware{};

		// Common Members
		Camera m_Camera;
		RenderSettings m_Settings{};

		// Owned Data Vectors
		std::vector<Mesh> m_SolidMeshes{}; 
		std::vector<Mesh> m_AlphaMeshes{};
		std::vector<dae::Texture*> m_pSolidTextures{};
		std::vector<dae::Texture*> m_pAlphaTextures{};

		// Non-Owned Data Vectors
		std::vector<SolidEffect*> m_pSolidEffects{};
		std::vector<AlphaEffect*> m_pAlphaEffects{};

		// Dual Exclusive Members
		float m_RotTimer{};
		
		bool m_UseSoftware{ false };
		bool m_DoConsoleOut{ true };

		const Vector3 m_CAMERA_START_POS{ 0.0f, 0.0f, 0.0f };
	};
}