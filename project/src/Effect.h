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

// TODO : Rule Of 5?
class Effect
{
public:
	Effect(ID3D11Device* pDevice, const std::wstring& filePath);
	virtual ~Effect();

	ID3DX11Effect* const GetEffect() const { return m_pEffect; }
	ID3DX11EffectTechnique* const GetPoint() const { return m_pPoint; };
	ID3DX11EffectTechnique* const GetLinear() const { return m_pLinear; };
	ID3DX11EffectTechnique* const GetAnisotropic() const { return m_pAnisotropic; };

	ID3D11InputLayout* const GetInputLayout() const { return m_pInputLayout; };

	ID3DX11EffectMatrixVariable* const GetWorldViewProj() const { return m_pWorldViewProjMatrix; };

	static ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& filePath);

private:
	ID3DX11Effect* m_pEffect{};
	ID3DX11EffectTechnique* m_pPoint{};
	ID3DX11EffectTechnique* m_pLinear{};
	ID3DX11EffectTechnique* m_pAnisotropic{};

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