#pragma once

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
	Mesh(ID3D11Device* pDevice, Effect* pEffect, std::vector<Vertex> vertices, std::vector<uint32_t> indices);
	~Mesh();

	void Render(ID3D11DeviceContext* pDeviceContext, FilterTechnique technique = FilterTechnique::POINT) const; // Really hate that its not const ref

	Effect* GetEffect() { return m_pEffect; }

private:
	uint32_t m_NumIndices{};

	Effect* m_pEffect;

	dae::Texture* m_pDiffuse{};
	dae::Texture* m_pNormal{};
	dae::Texture* m_pSpecular{};
	dae::Texture* m_pGloss{};

	ID3D11Buffer* m_pVertexBuffer{};
	ID3D11Buffer* m_pIndexBuffer{};
};