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
#include "Mesh.h"

// Forward Declarations

namespace dae {
	class SoftwareRenderer;
	class DXRenderer;
	class Timer;

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

		// Cycles
		void CycleCullMode();
		void CycleSampleTechnique();
		void CycleLightingMode();
		void CycleRenderer();

		struct RenderSettings
		{
			// Shared
			bool doRotation{ false };
			bool doUniformBackGround{ false };
			CullMode cullMode{ CullMode::BACK_FACE };

			// Hardware
			bool doFireRender{ true };
			SampleTechnique texSamplingTechnique{ SampleTechnique::POINT };

			// Software
			LightingMode lightingMode{ LightingMode::COMBINED };
			bool doNormalMap{ true };
			bool doDepthVisual{ false };
			bool doBBoxVisual{ false };
		};

	private:
		void InitDirectX();
		void InitSoftware();

		// Renderers
		DXRenderer* m_pDirectX{};
		SoftwareRenderer* m_pSoftware{};

		// Common Members
		Camera m_Camera;
		std::vector<Mesh> m_Meshes{};
		RenderSettings m_Settings{};

		// Dual Exclusive Members
		float m_RotTimer{};
		bool m_UseSoftware{ false };
	};
}