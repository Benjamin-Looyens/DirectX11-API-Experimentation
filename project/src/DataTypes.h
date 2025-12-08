#pragma once

// Project Includes
#include "Vector3.h"
#include "Vector4.h"
#include "ColorRGB.h"

struct Vertex_PosCol
{
	dae::Vector3 pos{};
	dae::ColorRGB color{};
};

struct Vertex_Out
{
	dae::Vector4 pos{};
	dae::ColorRGB color{};
};
