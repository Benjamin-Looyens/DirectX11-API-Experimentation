#pragma once

// Project Includes
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix.h"
#include "ColorRGB.h"

// Basic Data Types
struct Vertex
{
	dae::Vector3 pos{};
	dae::Vector2 uv{};
	dae::ColorRGB color{};
	dae::Vector3 normal{};
	dae::Vector3 tangent{};
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

// Classes
namespace dae {
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
}

// Config Types
struct RenderSettings
{
	// Shared
	bool doRotation{ false };
	bool doUniformBackGround{ false };
	CullMode cullMode{ CullMode::BACK_FACE };
	dae::Transform transform{};

	// Hardware
	bool doFireRender{ true };
	SampleTechnique texSamplingTechnique{ SampleTechnique::POINT };

	// Software
	LightingMode lightingMode{ LightingMode::COMBINED };
	bool doNormalMap{ true };
	bool doDepthVisual{ false };
	bool doBBoxVisual{ false };
};