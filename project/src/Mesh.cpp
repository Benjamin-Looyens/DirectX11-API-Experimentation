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

Mesh::Mesh(ID3D11Device* pDevice, const std::wstring& effectFilePath, std::vector<Vertex_PosCol> vertices, std::vector<uint32_t> indices)
	: m_Effect{ pDevice, effectFilePath }
{
	// Create Vertex Buffer
	D3D11_BUFFER_DESC bufferDesc{};
	bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	bufferDesc.ByteWidth = sizeof(Vertex_PosCol) * static_cast<uint32_t>(vertices.size());
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData{};
	initData.pSysMem = vertices.data();

	HRESULT result = pDevice->CreateBuffer(&bufferDesc, &initData, &m_pVertexBuffer);
	if (FAILED(result)) return;

	// Create Index Buffer
	m_NumIndices = static_cast<uint32_t>(indices.size());
	bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	bufferDesc.ByteWidth = sizeof(uint32_t) * m_NumIndices;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	initData.pSysMem = indices.data();
	result = pDevice->CreateBuffer(&bufferDesc, &initData, &m_pIndexBuffer);
	if (FAILED(result)) return;
}

Mesh::~Mesh()
{
	if (m_pIndexBuffer) m_pIndexBuffer->Release();
	if (m_pVertexBuffer) m_pVertexBuffer->Release();
}

void Mesh::Render(ID3D11DeviceContext* pDeviceContext)
{
	// Set Primitive Topology
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Set Input Layout
	pDeviceContext->IASetInputLayout(m_Effect.GetInputLayout());

	// Set Vertex Buffer
	constexpr UINT stride = sizeof(Vertex_PosCol);
	constexpr UINT offset = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	// Set Index Buffer
	pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Draw
	D3DX11_TECHNIQUE_DESC techDesc{};
	m_Effect.GetTechnique()->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p) {
		m_Effect.GetTechnique()->GetPassByIndex(p)->Apply(0, pDeviceContext);
		pDeviceContext->DrawIndexed(m_NumIndices, 0, 0);
	}
}