// Project Includes
#include "DualRasterizer.h"
#include "Mesh.h"
#include "DirectXRenderer.h"

// External Includes


// Standard Includes
#include <iostream>

dae::DualRasterizer::DualRasterizer(SDL_Window* pWindow)
{
	m_pDirectX = new DXRenderer(pWindow);

	int width, height;

	//Initialize
	SDL_GetWindowSize(pWindow, &width, &height);

	// Camera Initialization
	m_Camera.Initialize(width, height, 45.0f, { 0.0f, 0.0f, -50.0f });
	// DirectX Initialization
	InitDirectX();
	// Software Initialization
	InitSoftware();
}

dae::DualRasterizer::~DualRasterizer()
{
	delete m_pDirectX;
	delete m_pSoftware;
}

void dae::DualRasterizer::Update(const Timer* pTimer)
{
	m_Camera.Update(pTimer);

	m_pDirectX->Update(pTimer, m_Camera);
}

void dae::DualRasterizer::Render() const
{
	// TODO : Implement DirectX & Software as classes
	//if (m_UseSoftware) m_pSoftware->Render(m_Settings, m_Camera, m_Meshes);
	//else m_pDirectX->Render(m_Settings, m_Camera, m_Meshes);

	m_pDirectX->Render();
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

	// HACK : Temporarily calling directly
	m_pDirectX->ToggleRotation();
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

	// HACK : Temporarily calling directly
	m_pDirectX->CycleFiltering();
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
	// TODO : Variable control over which renderer to use (hardware vs software)
}

void dae::DualRasterizer::InitDirectX()
{
}

void dae::DualRasterizer::InitSoftware()
{
}
