#pragma once

// Standard Includes
#include <string>

// Forward Declarations
struct ID3D11Device;
struct ID3DX11Effect;
struct ID3DX11EffectTechnique;
struct ID3D11InputLayout;
struct ID3DX11EffectMatrixVariable;
struct ID3DX11EffectShaderResourceVariable;

namespace dae {
	class Texture;
};

class Effect final
{
public:
	Effect(ID3D11Device* pDevice, const std::wstring& filePath);
	~Effect();

	ID3DX11Effect* const GetEffect() const { return m_pEffect; }
	ID3DX11EffectTechnique* const GetPoint() const { return m_pPoint; };
	ID3DX11EffectTechnique* const GetLinear() const { return m_pLinear; };
	ID3DX11EffectTechnique* const GetAnisotropic() const { return m_pAnisotropic; };

	ID3D11InputLayout* const GetInputLayout() const { return m_pInputLayout; };

	ID3DX11EffectVectorVariable* const GetCameraPos() const { return m_pCameraPos; };

	ID3DX11EffectMatrixVariable* const GetWorld() const { return m_pWorldMatrix; };
	ID3DX11EffectMatrixVariable* const GetWorldViewProj() const { return m_pWorldViewProjMatrix; };

	void SetDiffuseMap(dae::Texture* pDiffuseTexture);
	void SetNormalMap(dae::Texture* pNormalTexture);
	void SetSpecularMap(dae::Texture* pSpecularTexture);
	void SetGlossinessMap(dae::Texture* pGlossinessTexture);

	static ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& filePath);

private:
	ID3DX11Effect* m_pEffect{};
	ID3DX11EffectTechnique* m_pPoint{};
	ID3DX11EffectTechnique* m_pLinear{};
	ID3DX11EffectTechnique* m_pAnisotropic{};

	ID3D11InputLayout* m_pInputLayout{};
	
	// Vectors
	ID3DX11EffectVectorVariable* m_pCameraPos{};

	// Matrices
	ID3DX11EffectMatrixVariable* m_pWorldMatrix{};
	ID3DX11EffectMatrixVariable* m_pWorldViewProjMatrix{};

	// Textures
	ID3DX11EffectShaderResourceVariable* m_pDiffuseMap{};
	ID3DX11EffectShaderResourceVariable* m_pNormalMap{};
	ID3DX11EffectShaderResourceVariable* m_pSpecularMap{};
	ID3DX11EffectShaderResourceVariable* m_pGlossMap{};

};