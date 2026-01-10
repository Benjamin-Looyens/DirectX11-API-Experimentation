#pragma once

// Standard Includes
#include <string>

// Project Includes
#include "DataTypes.h"

// Forward Declarations
struct ID3D11Device;
struct ID3DX11Effect;
struct ID3DX11EffectTechnique;
struct ID3D11InputLayout;
struct ID3DX11EffectMatrixVariable;
struct ID3DX11EffectShaderResourceVariable;

namespace dae {
	class Texture;

	class Effect
	{
	public:
		Effect(ID3D11Device* pDevice, const std::wstring& filePath);
		virtual ~Effect();

		ID3DX11Effect* const GetEffect() const { return m_pEffect; }
		ID3DX11EffectTechnique* const GetTechnique() const { return m_pTechnique; };
		ID3D11InputLayout* const GetInputLayout() const { return m_pInputLayout; };
		ID3DX11EffectMatrixVariable* const GetWorldViewProj() const { return m_pWorldViewProjMatrix; };

		void SetSampleState(SampleTechnique tech);

		static ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& filePath);

		Effect(const Effect& other) = delete;
		Effect(const Effect&& other) = delete;
		Effect& operator=(const Effect& other) = delete;
		Effect& operator=(const Effect&& other) = delete;

	protected:
		void SetInputLayout(ID3D11InputLayout* pNewLayout);

	private:
		ID3DX11Effect* m_pEffect{};
		ID3DX11EffectTechnique* m_pTechnique{};

		ID3DX11EffectSamplerVariable* m_pActiveSampler{};
		ID3D11SamplerState* m_pPointSampler{};
		ID3D11SamplerState* m_pLinearSampler{};
		ID3D11SamplerState* m_pAnisotropicSampler{};

		ID3D11InputLayout* m_pInputLayout{};
	
		// Matrices
		ID3DX11EffectMatrixVariable* m_pWorldViewProjMatrix{};
	};

	class SolidEffect final : public Effect
	{
	public:
		SolidEffect(ID3D11Device* pDevice, const std::wstring& filePath);
		~SolidEffect() override;

		void SetDiffuseMap(dae::Texture* pDiffuseTexture);
		void SetNormalMap(dae::Texture* pNormalTexture);
		void SetSpecularMap(dae::Texture* pSpecularTexture);
		void SetGlossinessMap(dae::Texture* pGlossinessTexture);

		ID3DX11EffectVectorVariable* const GetCameraPos() const { return m_pCameraPos; };
		ID3DX11EffectMatrixVariable* const GetWorld() const { return m_pWorldMatrix; };

		SolidEffect(const SolidEffect& other) = delete;
		SolidEffect(const SolidEffect&& other) = delete;
		SolidEffect& operator=(const SolidEffect& other) = delete;
		SolidEffect& operator=(const SolidEffect&& other) = delete;

	private:
		// Variables
		ID3DX11EffectVectorVariable* m_pCameraPos{};
		ID3DX11EffectMatrixVariable* m_pWorldMatrix{};

		// Textures
		ID3DX11EffectShaderResourceVariable* m_pDiffuseMap{};
		ID3DX11EffectShaderResourceVariable* m_pNormalMap{};
		ID3DX11EffectShaderResourceVariable* m_pSpecularMap{};
		ID3DX11EffectShaderResourceVariable* m_pGlossMap{};
	};

	class AlphaEffect final : public Effect
	{
	public:
		AlphaEffect(ID3D11Device* pDevice, const std::wstring& filePath);
		~AlphaEffect() override;

		void SetDiffuseMap(dae::Texture* pDiffuseTexture);

		AlphaEffect(const AlphaEffect& other) = delete;
		AlphaEffect(const AlphaEffect&& other) = delete;
		AlphaEffect& operator=(const AlphaEffect& other) = delete;
		AlphaEffect& operator=(const AlphaEffect&& other) = delete;

	private:
		// Textures
		ID3DX11EffectShaderResourceVariable* m_pDiffuseMap{};
	};
};