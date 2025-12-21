// Behavior Defines
#define NOMINMAX // Necessary for not causing std::min and std::max conflicts for ColorRGB

// External Includes
#include <dxgi.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3dx11effect.h>

// Standard Includes
#include <sstream>
#include <iostream>
#include <assert.h>

// Project Includes
#include "Mesh.h"
#include "Matrix.h"
#include "Texture.h"
#include "Effect.h"


Mesh::Mesh(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices)
	: m_Vertices{ vertices }, m_Indices{ indices }
{

}

Mesh::Mesh(ID3D11Device* pDevice, Effect* pEffect, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices) 
	: Mesh(vertices, indices)
{
	InitDirectX(pDevice);

	// HACK : Kinda eghghghg
	m_pEffect = pEffect;
}

Mesh::~Mesh()
{
	if (m_pEffect) delete m_pEffect;

	if (m_pIndexBuffer) m_pIndexBuffer->Release();
	if (m_pVertexBuffer) m_pVertexBuffer->Release();
}

HRESULT Mesh::InitDirectX(ID3D11Device* pDevice)
{
	// Create Vertex Buffer
	D3D11_BUFFER_DESC bufferDesc{};
	bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	bufferDesc.ByteWidth = sizeof(Vertex) * static_cast<uint32_t>(m_Vertices.size());
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData{};
	initData.pSysMem = m_Vertices.data();

	HRESULT result = pDevice->CreateBuffer(&bufferDesc, &initData, &m_pVertexBuffer);
	if (FAILED(result)) return result;

	// Create Index Buffer
	m_NumIndices = static_cast<uint32_t>(m_Indices.size());
	bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	bufferDesc.ByteWidth = sizeof(uint32_t) * m_NumIndices;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	initData.pSysMem = m_Indices.data();

	result = pDevice->CreateBuffer(&bufferDesc, &initData, &m_pIndexBuffer);
	if (FAILED(result)) return result;

	return result; // TODO : Check if need to do anything else before returning
}

// TODO : Store Effect outside of mesh, then pass effect to the render function and use it
// TODO : Differentiate DirectXRender & Normal Render? (in my software I use the mesh data not a render call)
void Mesh::Render(ID3D11DeviceContext* pDeviceContext) const
{
	// Set Primitive Topology
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Set Input Layout
	pDeviceContext->IASetInputLayout(m_pEffect->GetInputLayout());

	// Set Vertex Buffer
	constexpr UINT stride = sizeof(Vertex);
	constexpr UINT offset = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	// Set Index Buffer
	pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Draw
	D3DX11_TECHNIQUE_DESC techDesc{};
	ID3DX11EffectTechnique* pTechnique = m_pEffect->GetTechnique();
	pTechnique->GetDesc(&techDesc);
	for (UINT index{ }; index < techDesc.Passes; ++index) {
		pTechnique->GetPassByIndex(index)->Apply(0, pDeviceContext);
		pDeviceContext->DrawIndexed(m_NumIndices, 0, 0);
	}
}

void Mesh::SetEffect(Effect* pNewEffect)
{
	if (m_pEffect)  delete m_pEffect;
	m_pEffect = pNewEffect;
}
