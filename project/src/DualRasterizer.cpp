// Project Includes
#include "DualRasterizer.h"
#include "DirectXRenderer.h"

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
	m_Camera.Initialize(width, height, 45.0f, { 0.0f, 0.0f, -50.0f });

	// Texture Initialization
	std::string vehicleDiffusePath{ "resources/vehicle_diffuse.png" };
	std::string vehicleNormalPath{ "resources/vehicle_normal.png" };
	std::string vehicleSpecularPath{ "resources/vehicle_specular.png" };
	std::string vehicleGlossPath{ "resources/vehicle_gloss.png" };
	std::string fireDiffusePath{ "resources/fireFX_diffuse.png" };

	dae::Texture* pDiffuse =		dae::Texture::LoadFromFile(vehicleDiffusePath);
	dae::Texture* pNormal =			dae::Texture::LoadFromFile(vehicleNormalPath);
	dae::Texture* pSpecular =		dae::Texture::LoadFromFile(vehicleSpecularPath);
	dae::Texture* pGloss =			dae::Texture::LoadFromFile(vehicleGlossPath);
	dae::Texture* pDiffuseFire =	dae::Texture::LoadFromFile(fireDiffusePath);

	m_pTextures.push_back(pDiffuse);
	m_pTextures.push_back(pNormal);
	m_pTextures.push_back(pSpecular);
	m_pTextures.push_back(pGloss);

	m_pTextures.push_back(pDiffuseFire);

	// Mesh Initialization
	// TODO : Store the vertices & indices within the mesh (need for software rasterizer)
	std::vector<Vertex> vehicleVerts{};
	std::vector<uint32_t> vehicleIndices{};
	Utils::ParseOBJ("resources/vehicle.obj", vehicleVerts, vehicleIndices);
	m_SolidMeshes.emplace_back(vehicleVerts, vehicleIndices); // Vehicle

	std::vector<Vertex> fireVerts{};
	std::vector<uint32_t> fireIndices{};
	Utils::ParseOBJ("resources/fireFX.obj", fireVerts, fireIndices);
	// HACK : Will copy FULL vertex into input layout, even though it only uses POS and UV
	m_AlphaMeshes.emplace_back(fireVerts, fireIndices); // Fire

	// DirectX Initialization
	m_pDirectX = new DXRenderer(pWindow);
	InitDirectX();

	// Software Initialization
	//m_pSoftware = new SoftwareRenderer(pWindow);
	InitSoftware();
}

dae::DualRasterizer::~DualRasterizer()
{
	delete m_pDirectX;
	delete m_pSoftware;

	for (Texture* pTexture : m_pTextures)
		delete pTexture;
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
	//if (m_UseSoftware) m_pSoftware->Update(pTimer, m_Camera, m_SolidMeshes, m_AlphaMeshes, m_Settings);
	//else m_pDirectX->Update(pTimer, m_Camera, m_SolidMeshes, m_AlphaMeshes, m_Settings);
	
	m_pDirectX->Update(pTimer, m_Camera, m_SolidMeshes, m_AlphaMeshes, m_Settings);
}

void dae::DualRasterizer::Render() const
{
	// TODO : Implement DirectX & Software as classes
	//if (m_UseSoftware) m_pSoftware->Render(m_Settings, m_Camera, m_Meshes);
	//else m_pDirectX->Render(m_Settings, m_Camera, m_Meshes);

	m_pDirectX->Render(m_SolidMeshes, m_AlphaMeshes, m_Settings);
}

void dae::DualRasterizer::ToggleRotation()
{
	m_Settings.doRotation = !m_Settings.doRotation;

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

// Still really dont like how I cycle enums but welp
void dae::DualRasterizer::CycleCullMode()
{
	m_Settings.cullMode = static_cast<CullMode>((static_cast<uint8_t>(m_Settings.cullMode) + 1) % static_cast<uint8_t>(CullMode::MODE_COUNT));
	
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

	switch (m_UseSoftware)
	{
	case true:
		std::cout << "SOFTWARE\n";
		break;
	case false:
		std::cout << "DIRECT X\n";
		break;
	}
}

void dae::DualRasterizer::InitDirectX()
{
	ID3D11Device* pDevice = m_pDirectX->GetDevice();

	HRESULT result;
	for (Texture* pTexture : m_pTextures) {
		result = pTexture->InitDirectX(pDevice);
		if (FAILED(result)); // TODO : Do something if failed
	}

	for (Mesh& mesh : m_SolidMeshes) {
		result = mesh.InitDirectX(pDevice);
		if (FAILED(result)); // TODO : Do something if failed
	}

	for (Mesh& mesh : m_AlphaMeshes) {
		result = mesh.InitDirectX(pDevice);
		if (FAILED(result)); // TODO : Do something if failed
	}

	// Create & Add Effects
	std::wstring vehicleEffectPath{ L"resources/PosCol3D.fx" };
	SolidEffect* pVehicleEffect = new SolidEffect(pDevice, vehicleEffectPath);

	// HACK : Temporarily accessing by index (NEED TO FIX!!)
	pVehicleEffect->SetDiffuseMap(m_pTextures[0]);
	pVehicleEffect->SetNormalMap(m_pTextures[1]);
	pVehicleEffect->SetSpecularMap(m_pTextures[2]);
	pVehicleEffect->SetGlossinessMap(m_pTextures[3]);

	std::wstring fireEffectPath{ L"resources/Fire.fx" };
	AlphaEffect* pFireEffect = new AlphaEffect(pDevice, fireEffectPath);

	// HACK : Temporarily accessing by index (NEED TO FIX!!)
	pFireEffect->SetDiffuseMap(m_pTextures[4]);

	// Effect -> Mesh Linking
	// HACK : Temporarily accessing by index (NEED TO FIX!!)
	m_SolidMeshes[0].SetEffect(pVehicleEffect);
	m_AlphaMeshes[0].SetEffect(pFireEffect);
}

void dae::DualRasterizer::InitSoftware()
{
}
