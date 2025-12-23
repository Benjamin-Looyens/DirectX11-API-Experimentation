#pragma once

// External Includes
#define NOMINMAX // Necessary for not causing std::min and std::max conflicts
#include <Windows.h> // For HRESULT
#include <SDL_surface.h>

// Standard Includes
#include <string>

// Project Includes
#include "ColorRGB.h"

// Forward Declarations
struct ID3D11Texture2D;
struct ID3D11ShaderResourceView;
struct ID3D11Device;

namespace dae
{
	struct Vector2;

	class Texture final
	{
	public:
		~Texture();

		static Texture* LoadFromFile(const std::string& path);

		HRESULT InitDirectX(ID3D11Device* pDevice);
		ID3D11ShaderResourceView* GetSurfaceView() const { return m_pResourceView; };

		ColorRGB SoftwareSample(const Vector2& uv) const;

	private:
		Texture(SDL_Surface* pSurface);

		// SDL
		SDL_Surface* m_pSurface{ nullptr };
		uint32_t* m_pSurfacePixels{ nullptr };

		// DirectX
		bool m_HasInitializedDirectX{ false };
		ID3D11Texture2D* m_pResource{};
		ID3D11ShaderResourceView* m_pResourceView{};
	};
}