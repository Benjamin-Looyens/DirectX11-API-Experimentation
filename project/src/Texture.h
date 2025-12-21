#pragma once
#include <SDL_surface.h>
#include <string>
#include "ColorRGB.h"
#include <winnt.h> // For HRESULT

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

		// TODO : Add SDL sample for software

		ID3D11ShaderResourceView* GetSurfaceView() const { return m_pResourceView; };

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