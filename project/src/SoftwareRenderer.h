#pragma once
#include <cstdint>
#include <vector>

#include "Camera.h"
#include "DataTypes.h"
#include "Mesh.h"

struct SDL_Window;
struct SDL_Surface;

namespace dae
{
	class Timer;

	class SoftwareRenderer final
	{
	public:
		SoftwareRenderer(SDL_Window* pWindow);
		~SoftwareRenderer();

		SoftwareRenderer(const SoftwareRenderer&) = delete;
		SoftwareRenderer(SoftwareRenderer&&) noexcept = delete;
		SoftwareRenderer& operator=(const SoftwareRenderer&) = delete;
		SoftwareRenderer& operator=(SoftwareRenderer&&) noexcept = delete;

		void Update(const Timer* pTimer, dae::Camera& cam, std::vector<Mesh>& solidMeshes, std::vector<Mesh>& alphaMeshes, ::RenderSettings& settings);
		void Render(const std::vector<Mesh>& solidMeshes, const std::vector<Mesh>& alphaMeshes, const ::RenderSettings& settings);

		bool SaveBufferToImage() const;

		void ToggleBoundsRender();
		void ToggleWireFrameRender();
		void ToggleDepthRender();
		void ToggleRotation();
		void ToggleNormalMap();

		void CycleLightingMode();

		void ResetCamera();

		enum class LightingMode : uint8_t
		{
			ObservedArea,
			Diffuse,
			Specular,
			Combined,

			ModeAmount
		};

	private:
		void VertexTransformationFunction(const Matrix& worldMat, const Camera& cam, const std::vector<Vertex>& vertices_in, std::vector<Vertex_Out>& vertices_out) const;
		void UpdateColor(const uint32_t& index, ColorRGB& col); // HACK : Not referenced as const, is kinda bad practice

		LightingMode m_CurMode{ LightingMode::Combined };

		SDL_Window* m_pWindow{};

		SDL_Surface* m_pFrontBuffer{ nullptr };
		SDL_Surface* m_pBackBuffer{ nullptr };
		uint32_t* m_pBackBufferPixels{};
		
		float* m_pDepthBuffer{};

		int m_Width{};
		int m_Height{};

		bool m_DoWireFrameRender{ false };
		bool m_DoBoundsRender{ false };
		bool m_DoDepthRender{ false };
		bool m_DoRotation{ false };
		bool m_UseNormalMap{ true };

		bool m_VerticesDirty{ true };
	};
}
