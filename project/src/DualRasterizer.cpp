// Project Includes
#include "DualRasterizer.h"
#include "DirectXRenderer.h"
#include "SoftwareRenderer.h"

#include "Utils.h"

#include "Texture.h"
#include "Mesh.h"
#include "Effect.h"

// External Includes


// Standard Includes
#include <iostream>

dae::DualRasterizer::DualRasterizer(SDL_Window* pWindow)
{
	int width, height;
	SDL_GetWindowSize(pWindow, &width, &height);

	// Camera Initialization
	m_Camera.Initialize(width, height, 45.0f, m_CAMERA_START_POS);
	m_Camera.nearPlane = 0.1f;
	m_Camera.farPlane = 100.0f;

	// Texture Initialization
	std::string vehicleDiffusePath{ "resources/vehicle_diffuse.png" };
	std::string vehicleNormalPath{ "resources/vehicle_normal.png" };
	std::string vehicleSpecularPath{ "resources/vehicle_specular.png" };
	std::string vehicleGlossPath{ "resources/vehicle_gloss.png" };
	std::string fireDiffusePath{ "resources/fireFX_diffuse.png" };

	// Solid textures
	dae::Texture* pDiffuse =	dae::Texture::LoadFromFile(vehicleDiffusePath);
	dae::Texture* pNormal =		dae::Texture::LoadFromFile(vehicleNormalPath);
	dae::Texture* pSpecular =	dae::Texture::LoadFromFile(vehicleSpecularPath);
	dae::Texture* pGloss =		dae::Texture::LoadFromFile(vehicleGlossPath);

	m_pSolidTextures.push_back(pDiffuse);
	m_pSolidTextures.push_back(pNormal);
	m_pSolidTextures.push_back(pSpecular);
	m_pSolidTextures.push_back(pGloss);

	// Alpha Textures
	dae::Texture* pDiffuseFire = dae::Texture::LoadFromFile(fireDiffusePath);

	m_pAlphaTextures.push_back(pDiffuseFire);

	// Mesh Initialization
	std::vector<Vertex> vehicleVerts{};
	std::vector<uint32_t> vehicleIndices{};
	Utils::ParseOBJ("resources/vehicle.obj", vehicleVerts, vehicleIndices);
	m_SolidMeshes.emplace_back(vehicleVerts, vehicleIndices); // Vehicle

	std::vector<Vertex> fireVerts{};
	std::vector<uint32_t> fireIndices{};
	Utils::ParseOBJ("resources/fireFX.obj", fireVerts, fireIndices);
	m_AlphaMeshes.emplace_back(fireVerts, fireIndices); // Fire

	// Initialize Transform
	m_Settings.transform.SetTranslation({ 0.0f, 0.0f, 50.0f });

	// DirectX Initialization
	m_pDirectX = new DXRenderer(pWindow);
	InitDirectX();

	// Software Initialization
	m_pSoftware = new SoftwareRenderer(pWindow);
	InitSoftware();
}

dae::DualRasterizer::~DualRasterizer()
{
	m_AlphaMeshes.clear();
	m_SolidMeshes.clear();
	
	for (Texture* pTexture : m_pSolidTextures)
		delete pTexture;

	for (Texture* pTexture : m_pAlphaTextures)
		delete pTexture;

	delete m_pDirectX;
	delete m_pSoftware;
}

void dae::DualRasterizer::Update(const Timer* pTimer)
{
	// Update Own Members
	m_Camera.Update(pTimer);

	if (m_Settings.doRotation) {
		m_RotTimer += std::fmod(pTimer->GetElapsed(), PI_2);
		m_Settings.transform.SetRotationY(m_RotTimer);
	}

	// Update Current Renderer
	if (m_UseSoftware) m_pSoftware->Update(pTimer, m_Camera, m_SolidMeshes, m_AlphaMeshes, m_Settings);
	else m_pDirectX->Update(pTimer, m_Camera, m_SolidMeshes, m_AlphaMeshes, m_Settings);
}

void dae::DualRasterizer::Render() const
{
	// Render Current Renderer
	if (m_UseSoftware) m_pSoftware->Render(m_SolidMeshes, m_AlphaMeshes, m_Settings);
	else m_pDirectX->Render(m_SolidMeshes, m_AlphaMeshes, m_Settings);
}

void dae::DualRasterizer::ToggleRotation()
{
	m_Settings.doRotation = !m_Settings.doRotation;

	if (!m_DoConsoleOut) return;

	std::cout << "[SHARED] Mesh Rotation = ";
	switch (m_Settings.doRotation)
	{
	case true:
		std::cout << "ON\n";
		break;
	case false:
		std::cout << "OFF\n";
		break;
	}
}

void dae::DualRasterizer::ToggleUniformClear()
{
	m_Settings.doUniformBackGround = !m_Settings.doUniformBackGround;

	if (!m_DoConsoleOut) return;

	std::cout << "[SHARED] Uniform Background Clear = ";
	switch (m_Settings.doUniformBackGround)
	{
	case true:
		std::cout << "ON\n";
		break;
	case false:
		std::cout << "OFF\n";
		break;
	}
}

void dae::DualRasterizer::ToggleFireMesh()
{
	m_Settings.doFireRender = !m_Settings.doFireRender;

	if (!m_DoConsoleOut) return;

	std::cout << "[DIRECT X] Fire Mesh Rendering = ";
	switch (m_Settings.doFireRender)
	{
	case true:
		std::cout << "ON\n";
		break;
	case false:
		std::cout << "OFF\n";
		break;
	}
}

void dae::DualRasterizer::ToggleNormalMap()
{
	m_Settings.doNormalMap = !m_Settings.doNormalMap;

	if (!m_DoConsoleOut) return;

	std::cout << "[SOFTWARE] Normal Map Sampling = ";
	switch (m_Settings.doNormalMap)
	{
	case true:
		std::cout << "ON\n";
		break;
	case false:
		std::cout << "OFF\n";
		break;
	}
}

void dae::DualRasterizer::ToggleDepthRender()
{
	m_Settings.doDepthVisual = !m_Settings.doDepthVisual;

	if (!m_DoConsoleOut) return;

	std::cout << "[SOFTWARE] Depth Buffer Rendering = ";
	switch (m_Settings.doDepthVisual)
	{
	case true:
		std::cout << "ON\n";
		break;
	case false:
		std::cout << "OFF\n";
		break;
	}
}

void dae::DualRasterizer::ToggleBoundsRender()
{
	m_Settings.doBBoxVisual = !m_Settings.doBBoxVisual;

	if (!m_DoConsoleOut) return;

	std::cout << "[SOFTWARE] BoundingBox Rendering = ";
	switch (m_Settings.doBBoxVisual)
	{
	case true:
		std::cout << "ON\n";
		break;
	case false:
		std::cout << "OFF\n";
		break;
	}
}

void dae::DualRasterizer::ToggleConsoleOut()
{
	m_DoConsoleOut = !m_DoConsoleOut;

	std::cout << "[CONSOLE] Console Output = ";
	switch (m_DoConsoleOut)
	{
	case true:
		std::cout << "ON\n";
		break;
	case false:
		std::cout << "OFF\n";
		break;
	}
}

// Still really dont like how I cycle enums but welp
void dae::DualRasterizer::CycleCullMode()
{
	m_Settings.cullMode = static_cast<CullMode>((static_cast<uint8_t>(m_Settings.cullMode) + 1) % static_cast<uint8_t>(CullMode::MODE_COUNT));
	
	if (!m_DoConsoleOut) return;

	std::cout << "[SHARED] Cull Mode = ";
	switch (m_Settings.cullMode)
	{
	case CullMode::BACK_FACE:
		std::cout << "BACK-FACE\n";
		break;
	case CullMode::FRONT_FACE:
		std::cout << "FRONT-FACE\n";
		break;
	case CullMode::NONE:
		std::cout << "NONE\n";
		break;
	}
}

void dae::DualRasterizer::CycleSampleTechnique()
{
	m_Settings.texSamplingTechnique = static_cast<SampleTechnique>((static_cast<uint8_t>(m_Settings.texSamplingTechnique) + 1) % static_cast<uint8_t>(SampleTechnique::TECHNIQUE_COUNT));

	if (!m_DoConsoleOut) return;

	std::cout << "[DIRECT X] Filtering Technique = ";
	switch (m_Settings.texSamplingTechnique)
	{
	case SampleTechnique::POINT:
		std::cout << "POINT\n";
		break;
	case SampleTechnique::LINEAR:
		std::cout << "LINEAR\n";
		break;
	case SampleTechnique::ANISOTROPIC:
		std::cout << "ANISOTROPIC\n";
		break;
	}

	// Update Sampling for every Mesh/Effect combo
	for (Mesh& mesh : m_SolidMeshes) {
		mesh.GetEffect()->SetSampleState(m_Settings.texSamplingTechnique);
	}
	for (Mesh& mesh : m_AlphaMeshes) {
		mesh.GetEffect()->SetSampleState(m_Settings.texSamplingTechnique);
	}
}

void dae::DualRasterizer::CycleLightingMode()
{
	m_Settings.lightingMode = static_cast<LightingMode>((static_cast<uint8_t>(m_Settings.lightingMode) + 1) % static_cast<uint8_t>(LightingMode::MODE_COUNT));

	if (!m_DoConsoleOut) return;

	std::cout << "[SOFTWARE] Lighting Mode = ";
	switch (m_Settings.lightingMode)
	{
	case LightingMode::DIFFUSE:
		std::cout << "DIFFUSE\n";
		break;
	case LightingMode::OBSERVED_AREA:
		std::cout << "OBSERVED AREA\n";
		break;
	case LightingMode::SPECULAR:
		std::cout << "SPECULAR\n";
		break;
	case LightingMode::COMBINED:
		std::cout << "COMBINED\n";
		break;
	}
}

void dae::DualRasterizer::CycleRenderer()
{
	m_UseSoftware = !m_UseSoftware;

	if (m_DoConsoleOut) std::cout << "[SHARED] Current Renderer = ";
	switch (m_UseSoftware)
	{
	case true:
		if (m_DoConsoleOut) std::cout << "SOFTWARE\n";
		// HACK : Should just fix the camera rotation...
		m_Camera.rotSpeed = m_SOFTWARE_CAM_SPEED;
		break;
	case false:
		if (m_DoConsoleOut) std::cout << "DIRECT X\n";
		// HACK : Should just fix the camera rotation...
		m_Camera.rotSpeed = m_HARDWARE_CAM_SPEED;
		break;
	}
}

void dae::DualRasterizer::ResetCamera()
{
	if (m_DoConsoleOut) std::cout << "[SHARED] Camera Position Reset\n";

	m_Camera.SetOrigin(m_CAMERA_START_POS);
	m_Camera.ResetOrientation();
}

void dae::DualRasterizer::InitDirectX()
{
	ID3D11Device* pDevice = m_pDirectX->GetDevice();

	HRESULT result;
	for (Texture* pTexture : m_pSolidTextures) {
		result = pTexture->InitDirectX(pDevice);
		if (FAILED(result)) assert(false && "Solid Texture DX Initialization Failed!");
	}

	for (Texture* pTexture : m_pAlphaTextures) {
		result = pTexture->InitDirectX(pDevice);
		if (FAILED(result)) assert(false && "Alpha Texture DX Initialization Failed!");
	}

	for (Mesh& mesh : m_SolidMeshes) {
		result = mesh.InitDirectX(pDevice);
		if (FAILED(result)) assert(false && "Solid Mesh DX Initialization Failed!");
	}

	for (Mesh& mesh : m_AlphaMeshes) {
		result = mesh.InitDirectX(pDevice);
		if (FAILED(result)) assert(false && "Alpha Mesh DX Initialization Failed!");
	}

	// Create & Add Solid Effects
	std::wstring vehicleEffectPath{ L"resources/PosCol3D.fx" };
	m_pSolidEffects.push_back(new SolidEffect(pDevice, vehicleEffectPath));

	// Link Solid Meshes -> Effects
	for (size_t index{}; index < m_SolidMeshes.size(); index++) {
		auto& pCurEffect = m_pSolidEffects[index];
		auto& curMesh = m_SolidMeshes[index];

		dae::Texture* pDiffuse =	m_pSolidTextures[0 + index];
		dae::Texture* pNormal =		m_pSolidTextures[1 + index];
		dae::Texture* pSpecular =	m_pSolidTextures[2 + index];
		dae::Texture* pGloss =		m_pSolidTextures[3 + index];

		pCurEffect->SetDiffuseMap(pDiffuse);
		pCurEffect->SetNormalMap(pNormal);
		pCurEffect->SetSpecularMap(pSpecular);
		pCurEffect->SetGlossinessMap(pGloss);

		curMesh.SetEffect(pCurEffect);
	}

	// Create & Add Alpha Effects
	std::wstring fireEffectPath{ L"resources/Fire.fx" };
	m_pAlphaEffects.push_back(new AlphaEffect(pDevice, fireEffectPath));

	// Link Alpha Meshes -> Effects
	for (size_t index{}; index < m_AlphaMeshes.size(); index++) {
		auto& pCurEffect = m_pAlphaEffects[index];
		auto& curMesh = m_AlphaMeshes[index];

		dae::Texture* pDiffuse = m_pSolidTextures[index];

		pCurEffect->SetDiffuseMap(m_pAlphaTextures[index]);

		curMesh.SetEffect(pCurEffect);
	}
}

void dae::DualRasterizer::InitSoftware()
{
	// Loop over each Mesh + Texture Pairings
	for (size_t index{}; index < m_SolidMeshes.size(); index++) {
		dae::Texture* pDiffuse =	m_pSolidTextures[0 + index];
		dae::Texture* pNormal =		m_pSolidTextures[1 + index];
		dae::Texture* pSpecular =	m_pSolidTextures[2 + index];
		dae::Texture* pGloss =		m_pSolidTextures[3 + index];

		m_SolidMeshes[index].SetTextures(TextureData{ pDiffuse, pNormal, pSpecular, pGloss });
	}
}
