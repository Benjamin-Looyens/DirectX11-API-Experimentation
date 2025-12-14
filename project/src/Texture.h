#pragma once
#include <SDL_surface.h>
#include <string>
#include "ColorRGB.h"

// Forward Declarations
struct ID3D11Texture2D;
struct ID3D11ShaderResourceView;
struct ID3D11Device;

namespace dae
{
	struct Vector2;

	// TODO : SDL vs DX Texture for combined Rasterizer
	class Texture final
	{
	public:
		~Texture();

		static Texture* LoadFromFile(ID3D11Device* pDevice, const std::string& path);

		ID3D11ShaderResourceView* GetSurfaceView() const { return m_pResourceView; };

	private:
		Texture(ID3D11Device* pDevice, SDL_Surface* pSurface);

		ID3D11Texture2D* m_pResource{};
		ID3D11ShaderResourceView* m_pResourceView{};
	};
}