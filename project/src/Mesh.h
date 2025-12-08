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

class Mesh
{
public:
	Mesh(ID3D11Device* pDevice, const std::wstring& effectFilePath, std::vector<Vertex_PosCol> vertices, std::vector<uint32_t> indices);
	~Mesh();

	void Render(ID3D11DeviceContext* pDeviceContext);

private:
	uint32_t m_NumIndices{};

	Effect m_Effect;

	ID3D11Buffer* m_pVertexBuffer{};
	ID3D11Buffer* m_pIndexBuffer{};
};