#pragma once

// External Includes
#include <winnt.h>

// Standard Includes
#include <string>
#include <vector>

// Project Includes
#include "DataTypes.h"

// Forward Declarations
struct ID3D11Device;
struct ID3D11Buffer;

class Effect;

namespace dae {
	class Texture;
	struct Matrix;
}

class Mesh
{
public:
	Mesh(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);
	Mesh(ID3D11Device* pDevice, Effect* pEffect, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);
	~Mesh();

	// Direct X
	HRESULT InitDirectX(ID3D11Device* pDevice);
	void SetEffect(Effect* pNewEffect); // Really kind of a mess to call InitDirectX -> SetEffect
	Effect* GetEffect() { return m_pEffect; }
	void Render(ID3D11DeviceContext* pDeviceContext) const; // Really hate that its not const ref

private:
	// Software / SDL
	std::vector<Vertex> m_Vertices;
	std::vector<uint32_t> m_Indices;

	// Direct X
	uint32_t m_NumIndices{}; // TODO : When allowing software rasterizer, get rid of this and just use indices.size()
	Effect* m_pEffect{};
	ID3D11Buffer* m_pVertexBuffer{};
	ID3D11Buffer* m_pIndexBuffer{};
};