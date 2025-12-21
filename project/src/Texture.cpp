// Behavior Defines
#define NOMINMAX // Necessary for not causing std::min and std::max conflicts for ColorRGB

// External Includes
#include <dxgi.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3dx11effect.h>

#include <SDL_image.h>

// Standard Includes
#include <assert.h>

// Project Includes
#include "Texture.h"
#include "Vector2.h"

namespace dae
{
	Texture::Texture(SDL_Surface* pSurface)
		: m_pSurface{ pSurface }, m_pSurfacePixels{ (uint32_t*)pSurface }
	{}

	Texture::~Texture()
	{
		// SDL
		if (m_pSurface) SDL_FreeSurface(m_pSurface);

		// DirectX
		if (m_HasInitializedDirectX) {
			if (m_pResourceView) m_pResourceView->Release();
			if (m_pResource) m_pResource->Release();
		}
	}

	Texture* Texture::LoadFromFile(const std::string& path)
	{
		SDL_Surface* pSurface = IMG_Load(path.c_str());

		if (!pSurface) {
			SDL_FreeSurface(pSurface);
			return nullptr;
		}

		return new Texture(pSurface);
	}

	HRESULT Texture::InitDirectX(ID3D11Device* pDevice)
	{
		if (m_HasInitializedDirectX) return 0; // TODO : Check if this is correct syntax for Success in HRESULT

		DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
		D3D11_TEXTURE2D_DESC desc{};
		desc.Width = m_pSurface->w;
		desc.Height = m_pSurface->h;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = format;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = m_pSurface->pixels;
		initData.SysMemPitch = static_cast<UINT>(m_pSurface->pitch);
		initData.SysMemSlicePitch = static_cast<UINT>(m_pSurface->h * m_pSurface->pitch);

		HRESULT result = pDevice->CreateTexture2D(&desc, &initData, &m_pResource);
		if (FAILED(result)) return result; // Texture: Texture2D failed to create

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.Format = format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;

		result = pDevice->CreateShaderResourceView(m_pResource, &srvDesc, &m_pResourceView);
		if (FAILED(result)) return result; // Texture: ResourceView failed to create

		m_HasInitializedDirectX = true;
		return result; // TODO : Check if I should return something else
	}
}