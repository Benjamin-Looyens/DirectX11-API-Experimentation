// External Includes
#include <dxgi.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3dx11effect.h>

// Standard Includes
#include <string>
#include <sstream>
#include <iostream>
#include <assert.h>

// Project Includes
#include "Effect.h"

Effect::Effect(ID3D11Device* pDevice, const std::wstring& filePath)
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

Effect::~Effect()
{
	if (m_pInputLayout) m_pInputLayout->Release();
	if (m_pTechnique) m_pTechnique->Release();
	if (m_pEffect) m_pEffect->Release();
}

 ID3DX11Effect* Effect::LoadEffect(ID3D11Device* pDevice, const std::wstring& filePath)
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