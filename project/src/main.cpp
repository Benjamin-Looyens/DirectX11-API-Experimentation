//External includes
#include "SDL.h"
#include "SDL_surface.h"
#include "SDL_image.h"
#include "SDL_syswm.h"
#undef main

//Standard includes
#include <iostream>

//Project includes
#include "Timer.h"
#include "DualRasterizer.h"
#if defined(_DEBUG)
	#include "LeakDetector.h"
#endif

#include "Utils.h"
#include "ConsoleColorDefines.h"

using namespace dae;

void ShutDown(SDL_Window* pWindow)
{
	SDL_DestroyWindow(pWindow);
	SDL_Quit();
}

void PrintInputs()
{
	Utils::SetConsoleOutColor(dualRasterColors::CONSOLE_COLOR);
	std::cout <<
		"\n**************** INPUTS ****************\n" <<
		"H   - Print Input Mappings\n" <<
		"O   - Toggle Console Output\n";

	Utils::SetConsoleOutColor(dualRasterColors::SHARED_COLOR);
	std::cout <<
		"**************** SHARED ****************\n" <<
		"R   - Reset Camera Position & Rotation\n" <<
		"F1  - Switch Between DirectX & Software\n" <<
		"F2  - Toggle Mesh Rotation\n" <<
		"F9  - Cycle Between Cull Modes\n" <<
		"F10 - Toggle Uniform Background Clear\n" <<
		"F11 - Toggle Printing FPS\n";

	Utils::SetConsoleOutColor(dualRasterColors::HARDWARE_COLOR);
	std::cout <<
		"*************** HARDWARE ***************\n" <<
		"F3  - Toggle Fire Mesh Rendering\n" <<
		"F4  - Cycle Sampling Method\n";

	Utils::SetConsoleOutColor(dualRasterColors::SOFTWARE_COLOR);
	std::cout <<
		"*************** SOFTWARE ***************\n" <<
		"F5  - Cycle Shading Modes\n" <<
		"F6  - Toggle Normal Map Sampling\n" <<
		"F7  - Toggle Depth Buffer Rendering\n" <<
		"F8  - Toggle Bounding Box Rendering\n" <<
		"****************************************\n\n";
}

int main(int argc, char* args[])
{
	//Unreferenced parameters
	(void)argc;
	(void)args;

	// Leak detection
	#if defined(_DEBUG)
		LeakDetector detector{};
	#endif

	//Create window + surfaces
	SDL_Init(SDL_INIT_VIDEO);

	const uint32_t width = 640;
	const uint32_t height = 480;

	SDL_Window* pWindow = SDL_CreateWindow(
		"Dual Rasterizer - **Benjamin Looyens 2GD11**",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		width, height, 0);

	if (!pWindow)
		return 1;

	//Initialize "framework"
	const auto pTimer = new Timer();
	const auto pRenderer = new DualRasterizer(pWindow);

	//Start loop
	pTimer->Start();
	float printTimer = 0.f;
	bool isLooping = true;
	bool doFps = true;

	PrintInputs();

	while (isLooping)
	{
		//--------- Get input events ---------
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_QUIT:
				isLooping = false;
				break;
			case SDL_KEYUP:
				if (e.key.keysym.scancode == SDL_SCANCODE_H)
					PrintInputs();
				if (e.key.keysym.scancode == SDL_SCANCODE_O)
					pRenderer->ToggleConsoleOut();
				if (e.key.keysym.scancode == SDL_SCANCODE_R)
					pRenderer->ResetCamera();
				if (e.key.keysym.scancode == SDL_SCANCODE_F1) // (Shared) Toggle between DirectX& Software Rasterizer(‘F1’)
					pRenderer->CycleRenderer();
				if (e.key.keysym.scancode == SDL_SCANCODE_F2) // (Shared) Toggle Rotation(On / Off) (‘F2’)
					pRenderer->ToggleRotation();
				if (e.key.keysym.scancode == SDL_SCANCODE_F3) // (Hardware Only) Toggle FireFX mesh(On / Off) (‘F3’)
					pRenderer->ToggleFireMesh();
				if (e.key.keysym.scancode == SDL_SCANCODE_F4) // (Hardware Only) Toggle between Texture Sampling States(point - linear - anisotropic) (‘F4’)
					pRenderer->CycleSampleTechnique();
				if (e.key.keysym.scancode == SDL_SCANCODE_F5) // (Software Only) Cycle Shading Mode(Combined / ObservedArea / Diffuse / Specular) (‘F5’)
					pRenderer->CycleLightingMode();
				if (e.key.keysym.scancode == SDL_SCANCODE_F6) // (Software Only) Toggle NormalMap(On / Off) (‘F6’)
					pRenderer->ToggleNormalMap();
				if (e.key.keysym.scancode == SDL_SCANCODE_F7) // (Software Only) Toggle DepthBuffer Visualization(On / Off) (‘F7’)
					pRenderer->ToggleDepthRender();
				if (e.key.keysym.scancode == SDL_SCANCODE_F8) // (Software Only) Toggle BoundingBox Visualization(On / Off) (‘F8’)
					pRenderer->ToggleBoundsRender();
				if (e.key.keysym.scancode == SDL_SCANCODE_F9) // (Shared) Cycle Cull Modes(back - face, front - face, none) (‘F9’)
					pRenderer->CycleCullMode();
				if (e.key.keysym.scancode == SDL_SCANCODE_F10) // (Shared) Toggle Uniform Clear Color(On / Off) (‘F10’)
					pRenderer->ToggleUniformClear();
				if (e.key.keysym.scancode == SDL_SCANCODE_F11) { // (Shared) Toggle Print FPS(On / Off) (‘F11’)
					doFps = !doFps;

					std::cout << "[SHARED] FPS = ";
					switch (doFps)
					{
					case true:
						std::cout << "ON\n";
						break;
					case false:
						std::cout << "OFF\n";
						break;
					}
				}

				break;
			default: ;
			}
		}

		//--------- Update ---------
		pRenderer->Update(pTimer);

		//--------- Render ---------
		pRenderer->Render();

		//--------- Timer ---------
		pTimer->Update();

		if (doFps) {
			printTimer += pTimer->GetElapsed();
			if (printTimer >= 1.f)
			{
				printTimer = 0.f;
				if (doFps) {
					Utils::SetConsoleOutColor(dualRasterColors::FPS_COLOR);
					std::cout << "dFPS: " << pTimer->GetdFPS() << std::endl;
				}
			}
		}
	}
	pTimer->Stop();

	Utils::SetConsoleOutColor(stdConsoleColors::WHITE);

	//Shutdown "framework"
	delete pRenderer;
	delete pTimer;

	ShutDown(pWindow);
	return 0;
}