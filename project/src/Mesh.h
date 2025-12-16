#pragma once

// Standard Includes
#include <string>
#include <vector>

// Project Includes
#include "Effect.h"
#include "DataTypes.h"

// Forward Declarations
struct ID3D11Device;
struct ID3D11Buffer;

namespace dae {
	class Texture;
	struct Matrix;
}

class Mesh
{
public:
	// TODO : Texture paths to single struct
	Mesh(ID3D11Device* pDevice, const std::wstring& effectFilePath, const std::string& diffuseMapPath, const std::string& normalMapPath, const std::string& specularMapPath, const std::string& glossMapPath, std::vector<Vertex> vertices, std::vector<uint32_t> indices);
	~Mesh();

	void Render(ID3D11DeviceContext* pDeviceContext, dae::Matrix& world, dae::Matrix& worldViewProj, FilterTechnique technique = FilterTechnique::POINT); // Really hate that its not const ref

private:
	uint32_t m_NumIndices{};

	Effect m_Effect;

	dae::Texture* m_pDiffuse{};
	dae::Texture* m_pNormal{};
	dae::Texture* m_pSpecular{};
	dae::Texture* m_pGloss{};

	ID3D11Buffer* m_pVertexBuffer{};
	ID3D11Buffer* m_pIndexBuffer{};
};