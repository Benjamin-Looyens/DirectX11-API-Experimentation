#pragma once

// Project Includes
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix.h"
#include "ColorRGB.h"
#include "Texture.h"

namespace dae {
	// Basic Data Types
	struct Vertex
	{
		Vector3 pos{};
		Vector2 uv{};
		ColorRGB color{ dae::colors::White };
		Vector3 normal{};
		Vector3 tangent{};
	};

	struct Vertex_Out
	{
		Vector4 pos{};
		Vector2 uv{};
		ColorRGB color{};
		Vector3 normal{};
		Vector3 tangent{};
		Vector3 viewDir{};
	};

	struct InterpolationData
	{
		InterpolationData(const Vector4& _pos0, const Vector4& _pos1, const Vector4& _pos2, const float& _w0, const float& _w1, const float& _w2, const float& _zDepth, const float& _wDepth)
			: pos0{ _pos0 }, pos1{ _pos1 }, pos2{ _pos2 }, w0{ _w0 }, w1{ _w1 }, w2{ _w2 }, zDepth{ _zDepth }, wDepth{ _wDepth }
		{};

		const Vector4& pos0;
		const Vector4& pos1;
		const Vector4& pos2;

		const float& w0;
		const float& w1;
		const float& w2;

		const float& zDepth;
		const float& wDepth;
	};

	struct TextureData {
		Texture* pDiffuse;
		Texture* pNormal;
		Texture* pSpecular;
		Texture* pGloss;
	};

	// Enums
	enum class SampleTechnique : uint8_t
	{
		POINT,
		LINEAR,
		ANISOTROPIC,

		TECHNIQUE_COUNT
	};

	enum class LightingMode : uint8_t
	{
		OBSERVED_AREA,
		DIFFUSE,
		SPECULAR,
		COMBINED,

		MODE_COUNT
	};

	enum class CullMode : uint8_t
	{
		BACK_FACE,
		FRONT_FACE,
		NONE,

		MODE_COUNT
	};

	enum class PrimitiveTopology : uint8_t
	{
		TriangleList,
		TriangleStrip
	};

	enum class VertexRenderMode : uint8_t
	{
		COLORS,
		TEXTURES
	};

	// Classes
	class Transform final
	{
	public:
		Transform() = default;
		Transform(const Vector3& _translation, const Vector3& _rotation, const Vector3& _scale)
			: m_Translation{ Matrix::CreateTranslation(_translation) }, m_Rotation{ Matrix::CreateRotation(_rotation) }, m_Scale{ Matrix::CreateScale(_scale) }
		{
		}

		void SetTranslation(const Vector3& _translation)
		{
			m_Translation = Matrix::CreateTranslation(_translation);
			m_FullDirty = true;
		}
		void SetRotation(const Vector3& _rotation)
		{
			m_Rotation = Matrix::CreateRotation(_rotation);
			m_FullDirty = true;
		}
		void SetRotationX(const float& pitchAngle)
		{
			m_Rotation = Matrix::CreateRotationX(pitchAngle);
			m_FullDirty = true;
		}
		void SetRotationY(const float& yawAngle)
		{
			m_Rotation = Matrix::CreateRotationY(yawAngle);
			m_FullDirty = true;
		}
		void SetRotationZ(const float& rollAngle)
		{
			m_Rotation = Matrix::CreateRotationZ(rollAngle);
			m_FullDirty = true;
		}
		void SetScale(const Vector3& _scale)
		{
			m_Scale = Matrix::CreateScale(_scale);
			m_FullDirty = true;
		}
		void SetScale(const float& univScale)
		{
			m_Scale = Matrix::CreateScale({ univScale, univScale, univScale });
			m_FullDirty = true;
		}

		inline const Matrix& GetTranslation() const { return m_Translation; }
		inline const Matrix& GetRotation() const { return m_Rotation; }
		inline const Matrix& GetScale() const { return m_Scale; }

		const Matrix& GetFull()
		{
			if (m_FullDirty) {
				m_Full = m_Scale * m_Rotation * m_Translation;
				m_FullDirty = false;
			}

			return m_Full;
		}

	private:
		Matrix m_Translation{ Matrix::CreateIdentity() };
		Matrix m_Rotation{ Matrix::CreateIdentity() };
		Matrix m_Scale{ Matrix::CreateIdentity() };
		Matrix m_Full{ Matrix::CreateIdentity() };

		bool m_FullDirty{};
	};

	// Config Types
	struct RenderSettings
	{
		// Shared
		bool doRotation{ false };
		bool doUniformBackGround{ false };
		ColorRGB uniformBackGround{ 0.1f };
		CullMode cullMode{ CullMode::BACK_FACE };
		Transform transform{};
	
		// Hardware
		bool doFireRender{ true };
		SampleTechnique texSamplingTechnique{ SampleTechnique::POINT };
	
		// Software
		LightingMode lightingMode{ LightingMode::COMBINED };
		bool doNormalMap{ true };
		bool doDepthVisual{ false };
		bool doBBoxVisual{ false };
	};
}