#pragma once

// External Includes
#define NOMINMAX // Necessary for not causing std::min and std::max conflicts
#include <Windows.h> // For HRESULT

// Standard Includes
#include <string>
#include <vector>

// Project Includes
#include "DataTypes.h"

// Forward Declarations
struct ID3D11Device;
struct ID3D11Buffer;
struct ID3D11DeviceContext;

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

	// Software 
	const std::vector<Vertex>& GetVertices() const { return m_Vertices; }
	const std::vector<uint32_t>& GetIndices() const { return m_Indices; }
	const std::vector<Vertex_Out>& GetVertsOut() const { return m_VerticesOut; }

	const TextureData& GetTextures() const { return m_Textures; };

	PrimitiveTopology GetTopology() const { return m_PrimTop; }
	VertexRenderMode GetVertMode() const { return m_VertMode; }

	std::vector<Vertex_Out>& GetMutableVertsOut() { return m_VerticesOut; } // **MUTABLE** Vertices Out

private:
	// Software / SDL
	TextureData m_Textures{};
	PrimitiveTopology m_PrimTop{ PrimitiveTopology::TriangleList }; // TODO : Able to set this somewhere
	VertexRenderMode m_VertMode{ VertexRenderMode::TEXTURES }; // TODO : Able to set this somewhere

	std::vector<Vertex> m_Vertices;
	std::vector<Vertex_Out> m_VerticesOut;
	std::vector<uint32_t> m_Indices;

	// Direct X
	uint32_t m_NumIndices{}; // TODO : When allowing software rasterizer, get rid of this and just use indices.size()
	Effect* m_pEffect{};
	ID3D11Buffer* m_pVertexBuffer{};
	ID3D11Buffer* m_pIndexBuffer{};
};