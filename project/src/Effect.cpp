// Behavior Defines
#define NOMINMAX // Necessary for not causing std::min and std::max conflicts for ColorRGB

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
#include "Texture.h"

Effect::Effect(ID3D11Device* pDevice, const std::wstring& filePath)
{
	// Initialize Underlying DX11 Effect
	m_pEffect = LoadEffect(pDevice, filePath);
	if (m_pEffect) {
		m_pPoint = m_pEffect->GetTechniqueByName("PointTechnique");
		m_pLinear = m_pEffect->GetTechniqueByName("LinearTechnique");
		m_pAnisotropic = m_pEffect->GetTechniqueByName("AnisotropicTechnique");
		m_pWorldViewProjMatrix = m_pEffect->GetVariableByName("gWorldViewProj")->AsMatrix();
		m_pDiffuseMap = m_pEffect->GetVariableByName("gDiffuse")->AsShaderResource();

		if (!m_pPoint->IsValid()) std::wcout << L"Point Technique not valid!\n";
		if (!m_pLinear->IsValid()) std::wcout << L"Linear Technique not valid!\n";
		if (!m_pAnisotropic->IsValid()) std::wcout << L"Anisotropic Technique not valid!\n";
		if (!m_pWorldViewProjMatrix->IsValid()) std::wcout << L"WorldViewProjection Matrix not valid!\n";
		if (!m_pDiffuseMap->IsValid()) std::wcout << L"DiffuseMap Texture not valid!\n";
	}

	// Initialize Vertex Layout Values
	static constexpr uint32_t numElements{ 5 };
	D3D11_INPUT_ELEMENT_DESC vertexDesc[numElements]{};

	vertexDesc[0].SemanticName = "POSITION";
	vertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[0].AlignedByteOffset = 0;
	vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[1].SemanticName = "COLOR";
	vertexDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[1].AlignedByteOffset = 12;
	vertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[2].SemanticName = "TEXCOORD";
	vertexDesc[2].Format = DXGI_FORMAT_R32G32_FLOAT;
	vertexDesc[2].AlignedByteOffset = 24;
	vertexDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[3].SemanticName = "NORMAL";
	vertexDesc[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[3].AlignedByteOffset = 32;
	vertexDesc[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[4].SemanticName = "TANGENT";
	vertexDesc[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[4].AlignedByteOffset = 44;
	vertexDesc[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	// Create Input Layout
	D3DX11_PASS_DESC passDesc{};
	m_pPoint->GetPassByIndex(0)->GetDesc(&passDesc); // HACK : The input layout should be the same for each technique, HOWEVER it is strange laying it out based on one of the techniques?

	const HRESULT result = pDevice->CreateInputLayout(vertexDesc, numElements, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &m_pInputLayout);
	if (FAILED(result)) assert(false && "Failed to Create Input Layout!");

}

Effect::~Effect()
{
	if (m_pDiffuseMap) m_pDiffuseMap->Release();
	if (m_pInputLayout) m_pInputLayout->Release();
	if (m_pAnisotropic) m_pPoint->Release();
	if (m_pLinear) m_pPoint->Release();
	if (m_pPoint) m_pPoint->Release();
	if (m_pEffect) m_pEffect->Release();
}

void Effect::SetDiffuseMap(dae::Texture* pDiffuseTexture)
{
	if (m_pDiffuseMap) m_pDiffuseMap->SetResource(pDiffuseTexture->GetSurfaceView());
}

ID3DX11Effect* Effect::LoadEffect(ID3D11Device* pDevice, const std::wstring& filePath)
{
	HRESULT result;
	ID3D10Blob* pErrorBlob{};
	ID3DX11Effect* pEffect{};

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
			for (unsigned int index{ 0 }; index < pErrorBlob->GetBufferSize(); ++index) {
				ss << pErrors[index];
			}

			OutputDebugStringW(ss.str().c_str());
			pErrorBlob->Release();
			pErrorBlob = nullptr;

			std::wcout << ss.str() << L"\n";
			return nullptr;
		}
		else {
			std::wstringstream ss;
			ss << L"EffectLoader: Failed to Create Effect From File!\nPath: " << filePath << L"\n";
			std::wcout << ss.str();
			return nullptr;
		}
	}

	return pEffect;
}