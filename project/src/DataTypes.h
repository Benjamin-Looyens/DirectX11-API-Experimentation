#pragma once
// External Includes
#include <dxgi.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3dx11effect.h>

// Standard Includes
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <assert.h>

// Project Includes
#include "Vector3.h"
#include "Vector4.h"
#include "ColorRGB.h"

class Effect final
{
public:
	Effect(ID3D11Device* pDevice, const std::wstring& filePath)
	{
		// Initialize Underlying DX11 Effect
		m_pEffect = LoadEffect(pDevice, filePath);
		if (m_pEffect) {
			m_pTechnique = m_pEffect->GetTechniqueByName("DefaultTechnique");

			if (!m_pTechnique->IsValid()) std::wcout << L"Technique not valid!\n";
		}

		// Initialize Vertex Layout Values
		static constexpr uint32_t numElements{ 2 };
		D3D11_INPUT_ELEMENT_DESC vertexDesc[numElements]{};

		vertexDesc[0].SemanticName = "POSITION";
		vertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		vertexDesc[0].AlignedByteOffset = 0;
		vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		vertexDesc[1].SemanticName = "COLOR";
		vertexDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		vertexDesc[1].AlignedByteOffset = 12;
		vertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		// Create Input Layout
		D3DX11_PASS_DESC passDesc{};
		m_pTechnique->GetPassByIndex(0)->GetDesc(&passDesc);

		const HRESULT result = pDevice->CreateInputLayout(vertexDesc, numElements, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &m_pInputLayout);
		if (FAILED(result)) assert(false && "Failed to Create Input Layout!");
	}

	~Effect()
	{
		if (m_pInputLayout) m_pInputLayout->Release();
		if (m_pTechnique) m_pTechnique->Release();
		if (m_pEffect) m_pEffect->Release();
	}

	ID3DX11Effect* const GetEffect() const { return m_pEffect; }
	ID3DX11EffectTechnique* const GetTechnique() const { return m_pTechnique; };
	ID3D11InputLayout* const GetInputLayout() const { return m_pInputLayout; };

	static ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& filePath)
	{
		HRESULT result;
		ID3D10Blob* pErrorBlob{};
		ID3DX11Effect* pEffect;

		DWORD shaderFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
		shaderFlags |= D3DCOMPILE_DEBUG;
		shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

		result = D3DX11CompileEffectFromFile(filePath.c_str(), nullptr, nullptr, shaderFlags, 0, pDevice, &pEffect, &pErrorBlob);

		if (FAILED(result)) {
			if (pErrorBlob) {
				const char* pErrors = static_cast<char*>(pErrorBlob->GetBufferPointer());

				std::wstringstream ss;
				for (unsigned int i = 0; i < pErrorBlob->GetBufferSize(); ++i)
					ss << pErrors[i];

				OutputDebugStringW(ss.str().c_str());
				pErrorBlob->Release();
				pErrorBlob = nullptr;

				std::wcout << ss.str() << std::endl;
				return nullptr;
			}
			else {
				std::wstringstream ss;
				ss << "EffectLoader: Failed to CreateEffectFromFile!\nPath: " << filePath;
				std::wcout << ss.str() << std::endl;
				return nullptr;
			}
		}

		return pEffect;
	}

private:
	ID3DX11Effect* m_pEffect{};
	ID3DX11EffectTechnique* m_pTechnique{};
	ID3D11InputLayout* m_pInputLayout{};
};

struct Vertex_PosCol
{
	dae::Vector3 pos{};
	dae::ColorRGB color{};
};

struct Vertex_Out
{
	dae::Vector4 pos{};
	dae::ColorRGB color{};
};

class Mesh
{
public:
	Mesh(ID3D11Device* pDevice, const std::wstring& effectFilePath, std::vector<Vertex_PosCol> vertices, std::vector<uint32_t> indices)
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

	~Mesh()
	{
		if (m_pIndexBuffer) m_pIndexBuffer->Release();
		if (m_pVertexBuffer) m_pVertexBuffer->Release();
	}

	void Render(ID3D11DeviceContext* pDeviceContext)
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

private:
	uint32_t m_NumIndices{};

	Effect m_Effect;

	ID3D11Buffer* m_pVertexBuffer{};
	ID3D11Buffer* m_pIndexBuffer{};
};