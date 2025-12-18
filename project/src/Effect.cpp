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

#pragma region EffectBase
Effect::Effect(ID3D11Device* pDevice, const std::wstring& filePath)
{
	// Initialize Underlying DX11 Effect
	m_pEffect = LoadEffect(pDevice, filePath);
	if (m_pEffect) {
		// NOTE : Maybe get technique by index so they can have different names?
		m_pTechnique = m_pEffect->GetTechniqueByName("DefaultTechnique");
		if (!m_pTechnique->IsValid()) std::wcout << L"Technique for [" << filePath << L"] not valid!\n";

		m_pActiveSampler = m_pEffect->GetVariableByName("gCurSam")->AsSampler();
		if (!m_pTechnique->IsValid()) std::wcout << L"Active Sampler for [" << filePath << L"] not valid!\n";
		
		ID3DX11EffectSamplerVariable* pPoint = m_pEffect->GetVariableByName("gSamPoint")->AsSampler();
		ID3DX11EffectSamplerVariable* pLinear = m_pEffect->GetVariableByName("gSamLinear")->AsSampler();
		ID3DX11EffectSamplerVariable* pAnisotropic = m_pEffect->GetVariableByName("gSamAnisotropic")->AsSampler();
		
		if (!pAnisotropic->IsValid()) std::wcout << L"Anisotrpic Sampler for [" << filePath << L"] not valid!\n";
		else {
			pAnisotropic->GetSampler(0, &m_pAnisotropicSampler);
			pAnisotropic->Release();
		}
		if (!pLinear->IsValid()) std::wcout << L"Linear Sampler for [" << filePath << L"] not valid!\n";
		else {
			pLinear->GetSampler(0, &m_pLinearSampler);
			pLinear->Release();
		}
		if (!pPoint->IsValid()) std::wcout << L"Point Sampler for [" << filePath << L"] not valid!\n";
		else {
			pPoint->GetSampler(0, &m_pPointSampler);
			pPoint->Release();
			m_pActiveSampler->SetSampler(0, m_pPointSampler); // Set Base sampler
		}

		m_pWorldViewProjMatrix = m_pEffect->GetVariableByName("gWorldViewProj")->AsMatrix();
		if (!m_pWorldViewProjMatrix->IsValid()) std::wcout << L"WorldViewProjection Matrix [" << filePath << L"] not valid!\n";
	}
}

Effect::~Effect()
{
	if (m_pInputLayout) m_pInputLayout->Release();
	
	if (m_pWorldViewProjMatrix) m_pWorldViewProjMatrix->Release();

	if (m_pAnisotropicSampler) m_pAnisotropicSampler->Release();
	if (m_pLinearSampler) m_pLinearSampler->Release();
	if (m_pPointSampler) m_pPointSampler->Release();
	if (m_pActiveSampler) m_pActiveSampler->Release();

	if (m_pTechnique) m_pTechnique->Release();

	if (m_pEffect) m_pEffect->Release();
}

void Effect::SetInputLayout(ID3D11InputLayout* pNewLayout)
{
	if (m_pInputLayout) m_pInputLayout->Release();
	m_pInputLayout = pNewLayout;
}

void Effect::SetSampleState(SampleTechnique tech)
{
	switch (tech)
	{
	case SampleTechnique::POINT:
		m_pActiveSampler->SetSampler(0, m_pPointSampler);
		break;
	case SampleTechnique::LINEAR:
		m_pActiveSampler->SetSampler(0, m_pLinearSampler);
		break;
	case SampleTechnique::ANISOTROPIC:
		m_pActiveSampler->SetSampler(0, m_pAnisotropicSampler);
		break;
	}
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
#pragma endregion EffectBase

#pragma region EffectSolid
SolidEffect::SolidEffect(ID3D11Device* pDevice, const std::wstring& filePath) : Effect(pDevice, filePath)
{
	// Initialize Vertex Layout Values
	static constexpr uint32_t numElements{ 5 };
	D3D11_INPUT_ELEMENT_DESC vertexDesc[numElements]{};

	vertexDesc[0].SemanticName = "POSITION";
	vertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[0].AlignedByteOffset = 0;
	vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[1].SemanticName = "TEXCOORD";
	vertexDesc[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	vertexDesc[1].AlignedByteOffset = 12;
	vertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[2].SemanticName = "COLOR";
	vertexDesc[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[2].AlignedByteOffset = 20;
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
	GetTechnique()->GetPassByIndex(0)->GetDesc(&passDesc); // HACK : The input layout should be the same for each technique, HOWEVER it is strange laying it out based on one of the techniques?

	ID3D11InputLayout* pInputLayout{};
	const HRESULT result = pDevice->CreateInputLayout(vertexDesc, numElements, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &pInputLayout);
	if (FAILED(result)) assert(false && "Failed to Create Input Layout!");
	SetInputLayout(pInputLayout);

	ID3DX11Effect* pEffect = GetEffect();
	if (pEffect) {
		m_pDiffuseMap = pEffect->GetVariableByName("gDiffuse")->AsShaderResource();
		m_pNormalMap = pEffect->GetVariableByName("gNormal")->AsShaderResource();
		m_pSpecularMap = pEffect->GetVariableByName("gSpecular")->AsShaderResource();
		m_pGlossMap = pEffect->GetVariableByName("gGloss")->AsShaderResource();

		m_pCameraPos = pEffect->GetVariableByName("gCameraPos")->AsVector();
		m_pWorldMatrix = pEffect->GetVariableByName("gWorld")->AsMatrix();

		if (!m_pDiffuseMap->IsValid()) std::wcout << L"DiffuseMap Texture not valid!\n";
		if (!m_pNormalMap->IsValid()) std::wcout << L"NormalMap Texture not valid!\n";
		if (!m_pSpecularMap->IsValid()) std::wcout << L"SpecularMap Texture not valid!\n";
		if (!m_pGlossMap->IsValid()) std::wcout << L"GlossinessMap Texture not valid!\n";

		if (!m_pCameraPos->IsValid()) std::wcout << L"CameraPos not valid!\n";
		if (!m_pWorldMatrix->IsValid()) std::wcout << L"World Matrix not valid!\n";
	}
}

SolidEffect::~SolidEffect()
{
	if (m_pWorldMatrix) m_pWorldMatrix->Release();
	if (m_pCameraPos) m_pCameraPos->Release();

	if (m_pGlossMap) m_pGlossMap->Release();
	if (m_pSpecularMap) m_pSpecularMap->Release();
	if (m_pNormalMap) m_pNormalMap->Release();
	if (m_pDiffuseMap) m_pDiffuseMap->Release();
}

void SolidEffect::SetDiffuseMap(dae::Texture* pDiffuseTexture)
{
	if (m_pDiffuseMap) m_pDiffuseMap->SetResource(pDiffuseTexture->GetSurfaceView());
}

void SolidEffect::SetNormalMap(dae::Texture* pNormalTexture)
{
	if (m_pNormalMap) m_pNormalMap->SetResource(pNormalTexture->GetSurfaceView());
}

void SolidEffect::SetSpecularMap(dae::Texture* pSpecularTexture)
{
	if (m_pSpecularMap) m_pSpecularMap->SetResource(pSpecularTexture->GetSurfaceView());
}

void SolidEffect::SetGlossinessMap(dae::Texture* pGlossinessTexture)
{
	if (m_pGlossMap) m_pGlossMap->SetResource(pGlossinessTexture->GetSurfaceView());
}

#pragma endregion EffectSolid

#pragma region EffectAlpha
AlphaEffect::AlphaEffect(ID3D11Device* pDevice, const std::wstring& filePath)
	: Effect(pDevice, filePath)
{
	// Initialize Vertex Layout Values
	static constexpr uint32_t numElements{ 2 };
	D3D11_INPUT_ELEMENT_DESC vertexDesc[numElements]{};

	vertexDesc[0].SemanticName = "POSITION";
	vertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[0].AlignedByteOffset = 0;
	vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[1].SemanticName = "TEXCOORD";
	vertexDesc[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	vertexDesc[1].AlignedByteOffset = 12;
	vertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	// Create Input Layout
	D3DX11_PASS_DESC passDesc{};
	GetTechnique()->GetPassByIndex(0)->GetDesc(&passDesc); // HACK : The input layout should be the same for each technique, HOWEVER it is strange laying it out based on one of the techniques?

	ID3D11InputLayout* pInputLayout = GetInputLayout();
	const HRESULT result = pDevice->CreateInputLayout(vertexDesc, numElements, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &pInputLayout);
	if (FAILED(result)) assert(false && "Failed to Create Input Layout!");
	SetInputLayout(pInputLayout);

	ID3DX11Effect* pEffect = this->GetEffect();
	if (pEffect) {
		m_pDiffuseMap = pEffect->GetVariableByName("gDiffuse")->AsShaderResource();

		if (!m_pDiffuseMap->IsValid()) std::wcout << L"DiffuseMap Texture not valid!\n";
	}
}

AlphaEffect::~AlphaEffect()
{
	if (m_pDiffuseMap) m_pDiffuseMap->Release();
}

void AlphaEffect::SetDiffuseMap(dae::Texture* pDiffuseTexture)
{
	if (m_pDiffuseMap) m_pDiffuseMap->SetResource(pDiffuseTexture->GetSurfaceView());
}
#pragma endregion EffectAlpha