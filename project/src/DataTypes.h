#pragma once

// Project Includes
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "ColorRGB.h"

// Basic Data Types
struct Vertex
{
	dae::Vector3 pos{};
	dae::ColorRGB color{};
	dae::Vector2 uv{};
};

struct Vertex_Out
{
	dae::Vector4 pos{};
	dae::ColorRGB color{};
};

// Enums
enum class FilterTechnique
{
	POINT,
	LINEAR,
	ANISOTROPIC,

	TECHNIQUE_COUNT
};