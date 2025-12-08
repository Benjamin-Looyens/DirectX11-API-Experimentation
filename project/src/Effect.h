#pragma once

// Standard Includes
#include <string>

// Forward Declarations
struct ID3D11Device;
struct ID3DX11Effect;
struct ID3DX11EffectTechnique;
struct ID3D11InputLayout;

class Effect final
{
public:
	Effect(ID3D11Device* pDevice, const std::wstring& filePath);
	~Effect();

	ID3DX11Effect* const GetEffect() const { return m_pEffect; }
	ID3DX11EffectTechnique* const GetTechnique() const { return m_pTechnique; };
	ID3D11InputLayout* const GetInputLayout() const { return m_pInputLayout; };

	static ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& filePath);

private:
	ID3DX11Effect* m_pEffect{};
	ID3DX11EffectTechnique* m_pTechnique{};
	ID3D11InputLayout* m_pInputLayout{};
};