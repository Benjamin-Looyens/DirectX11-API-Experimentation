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

Mesh::Mesh(ID3D11Device* pDevice, Effect* pEffect, std::vector<Vertex> vertices, std::vector<uint32_t> indices) 
	: m_pEffect{ pEffect }
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
}

Mesh::~Mesh()
{
	if (m_pIndexBuffer) m_pIndexBuffer->Release();
	if (m_pVertexBuffer) m_pVertexBuffer->Release();

	delete m_pEffect;
}

void Mesh::Render(ID3D11DeviceContext* pDeviceContext, dae::Matrix& world, dae::Matrix& worldViewProj, dae::Vector3& camPos, FilterTechnique technique)
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

	// Update DirectX side variables
	m_pEffect->GetWorld()->SetMatrix(reinterpret_cast<float*>(&world));
	m_pEffect->GetWorldViewProj()->SetMatrix(reinterpret_cast<float*>(&worldViewProj));
	m_pEffect->GetCameraPos()->SetFloatVector(reinterpret_cast<float*>(&camPos));

	// Draw
	D3DX11_TECHNIQUE_DESC techDesc{};
	ID3DX11EffectTechnique* pTechnique{};

	switch (technique)
	{
	case FilterTechnique::POINT:
		pTechnique = m_pEffect->GetPoint();
		break;
	case FilterTechnique::LINEAR:
		pTechnique = m_pEffect->GetLinear();
		break;
	case FilterTechnique::ANISOTROPIC:
		pTechnique = m_pEffect->GetAnisotropic();
		break;
	}

	pTechnique->GetDesc(&techDesc);
	for (UINT index{ }; index < techDesc.Passes; ++index) {
		pTechnique->GetPassByIndex(index)->Apply(0, pDeviceContext);
		pDeviceContext->DrawIndexed(m_NumIndices, 0, 0);
	}
}
