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

Mesh::Mesh(ID3D11Device* pDevice, const std::wstring& effectFilePath, const std::string& diffuseMapPath, std::vector<Vertex> vertices, std::vector<uint32_t> indices)
	: m_Effect{ pDevice, effectFilePath }
{
	// Create Vertex Buffer
	D3D11_BUFFER_DESC bufferDesc{};
	bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	bufferDesc.ByteWidth = sizeof(Vertex) * static_cast<uint32_t>(vertices.size());
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

	m_pTexture = dae::Texture::LoadFromFile(pDevice, diffuseMapPath);
	m_Effect.SetDiffuseMap(m_pTexture);
}

Mesh::~Mesh()
{
	if (m_pIndexBuffer) m_pIndexBuffer->Release();
	if (m_pVertexBuffer) m_pVertexBuffer->Release();

	delete m_pTexture;
}

void Mesh::Render(ID3D11DeviceContext* pDeviceContext, dae::Matrix& worldViewProj, FilterTechnique technique)
{
	// Set Primitive Topology
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Set Input Layout
	pDeviceContext->IASetInputLayout(m_Effect.GetInputLayout());

	// Set Vertex Buffer
	constexpr UINT stride = sizeof(Vertex);
	constexpr UINT offset = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	// Set Index Buffer
	pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set WorldViewProjection Matrix
	m_Effect.GetWorldViewProj()->SetMatrix(reinterpret_cast<float*>(&worldViewProj));

	// Draw
	D3DX11_TECHNIQUE_DESC techDesc{};
	ID3DX11EffectTechnique* pTechnique{};

	switch (technique)
	{
	case FilterTechnique::POINT:
		pTechnique = m_Effect.GetPoint();
		break;
	case FilterTechnique::LINEAR:
		pTechnique = m_Effect.GetLinear();
		break;
	case FilterTechnique::ANISOTROPIC:
		pTechnique = m_Effect.GetAnisotropic();
		break;
	}

	pTechnique->GetDesc(&techDesc);
	for (UINT index{ }; index < techDesc.Passes; ++index) {
		pTechnique->GetPassByIndex(index)->Apply(0, pDeviceContext);
		pDeviceContext->DrawIndexed(m_NumIndices, 0, 0);
	}
}
