#include "Utils.h"

bool dae::Utils::FrustumCheck(const Vector4& v0, const Vector4& v1, const Vector4& v2)
{
	// Frustum Culling Z
	if (v0.z <= 0.0f || v0.z >= 1.0f) return false;
	if (v1.z <= 0.0f || v1.z >= 1.0f) return false;
	if (v2.z <= 0.0f || v2.z >= 1.0f) return false;

	// Frustum culling X
	if (v0.x <= -1.0f || v0.x >= 1.0f) return false;
	if (v1.x <= -1.0f || v1.x >= 1.0f) return false;
	if (v2.x <= -1.0f || v2.x >= 1.0f) return false;

	// Frustum culling Y
	if (v0.y <= -1.0f || v0.y >= 1.0f) return false;
	if (v1.y <= -1.0f || v1.y >= 1.0f) return false;
	if (v2.y <= -1.0f || v2.y >= 1.0f) return false;

	return true;
}

bool dae::Utils::PixelShade(const Vertex_Out& vert, const TextureData& meshTexture, const LightingMode& lightMode, const VertexRenderMode& vertMode, const Vector3& lightDir, bool useNormalMap, ColorRGB& color_OUT)
{
	constexpr float AMBIENT_VALUE = 0.025f;
	constexpr float LIGHT_INTENSITY = 7.0f; // NOTE : Could implement a Light Type that stores all the info for lights (direction, intensity, ambient)
	constexpr float SHININESS = 25.0f;

	// Normal Calc
	const Vector3 binormal = Vector3::Cross(vert.normal, vert.tangent);
	const Matrix tangentSpace = { vert.tangent, binormal, vert.normal, Vector3::Zero };

	Vector3 normalMapSample = { meshTexture.pNormal->SoftwareSample(vert.uv) };
	normalMapSample = 2.0f * normalMapSample - Vector3{ 1.0f, 1.0f, 1.0f };
	normalMapSample = tangentSpace.TransformVector(normalMapSample).Normalized();

	const Vector3& normal = useNormalMap ? normalMapSample : vert.normal;

	// Observed Area Calc
	const float observedArea = std::max(0.0f, Vector3::Dot(normal, lightDir));

	// Lambert calc
	const ColorRGB diffuse = vertMode == VertexRenderMode::COLORS ? vert.color : meshTexture.pDiffuse->SoftwareSample(vert.uv);
	const ColorRGB lambert = ((LIGHT_INTENSITY / PI) * diffuse);

	// Specular (Phong) Calc
	const float ks = meshTexture.pSpecular->SoftwareSample(vert.uv).r;
	const float exp = meshTexture.pGloss->SoftwareSample(vert.uv).r;

	const Vector3 reflect = lightDir - (2.0f * (Vector3::Dot(lightDir, normal) * normal));
	const float cosAngle = std::max(Vector3::Dot(reflect.Normalized(), vert.viewDir), 0.0f);
	const float specular = ks * std::powf(cosAngle, exp * SHININESS);

	// HACK : Ideally, only compute what is necessary instead of calculating it all before switch statement
	switch (lightMode)
	{
	case LightingMode::OBSERVED_AREA:
		color_OUT = { observedArea };
		break;
	case LightingMode::DIFFUSE:
		color_OUT = lambert *
			observedArea;
		break;
	case LightingMode::SPECULAR:
		color_OUT = { specular };
		break;
	case LightingMode::COMBINED:
		color_OUT = (lambert + specular) *
			observedArea +
			ColorRGB{ AMBIENT_VALUE };
		break;
	}

	return true;
}
