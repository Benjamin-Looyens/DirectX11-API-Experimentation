#pragma once

// Standard Includes
#include <cassert>

// External Includes
#include <SDL_keyboard.h>
#include <SDL_mouse.h>

// Project Includes
#include "Matrix.h"
#include "Vector3.h"
#include "Vector4.h"
#include "MathHelpers.h"
#include "Timer.h"

namespace dae
{
	struct Camera final
	{
		Camera() = default;
		Camera(const Vector3& _origin, float _fovAngle, int _width, int _height):
			origin{_origin},
			fovAngle{_fovAngle},
			aspectRatio{ static_cast<float>(_width) / static_cast<float>(_height) }
		{}

		Vector3 origin{};
		float fovAngle{90.f};
		float fov{ tanf((fovAngle * TO_RADIANS) / 2.f) };

		float aspectRatio{};

		float movSpeed{ 10.0f };
		float panSpeed{ 0.25f };
		float rotSpeed{ 0.0075f }; // Feels dubious that its such a small number but it works now

		Vector3 forward{Vector3::UnitZ};
		Vector3 up{Vector3::UnitY};
		Vector3 right{Vector3::UnitX};

		float totalPitch{};
		float totalYaw{};

		Matrix invViewMatrix{};
		Matrix viewMatrix{};
		bool viewDirty = true;

		Matrix projectMatrix{};
		bool projectDirty = true;

		bool updatedMatricesThisFrame = false;

		float nearPlane{ 1.0f };
		float farPlane{ 1000.0f };

		void Initialize(int _width, int _height, float _fovAngle = 90.f, Vector3 _origin = {0.f,0.f,0.f})
		{
			fovAngle = _fovAngle;
			fov = tanf((fovAngle * TO_RADIANS) / 2.f);

			origin = _origin;

			aspectRatio = static_cast<float>(_width) / static_cast<float>(_height);
		}

		void SetOrigin(const Vector3& newPos) {
			origin = newPos;
			viewDirty = true;
		}

		void ResetOrientation()
		{
			forward = Vector3::UnitZ;
			up = Vector3::UnitY;
			right = Vector3::UnitX;

			totalYaw = 0.0f;
			totalPitch = 0.0f;

			viewDirty = true;
			projectDirty = true;
		}

		void CalculateViewMatrix()
		{
			if (viewDirty) {
				invViewMatrix = CalculateCameraToWorld();
				viewMatrix = Matrix::Inverse(invViewMatrix);
				viewDirty = false;
				updatedMatricesThisFrame = true;
			}
		}

		void CalculateProjectionMatrix()
		{
			if (projectDirty) {
				const float farToNear = farPlane - nearPlane;
				const float a = farPlane / farToNear;
				const float b = -(farPlane * nearPlane) / farToNear;

				projectMatrix = {
					{ 1 / (aspectRatio * fov), 0, 0, 0},
					{ 0, 1 / fov, 0, 0},
					{ 0, 0, a, 1 }, // Left handed (so is {0, 0, a, 1} not {0, 0, a, -1}
					{ 0, 0, b, 0 }
				};

				projectDirty = false;
				updatedMatricesThisFrame = true;
			}
		}

		Matrix CalculateCameraToWorld()
		{
			right = Vector3::Cross(Vector3::UnitY, forward).Normalized();
			up = Vector3::Cross(forward, right).Normalized();

			return { right, up, forward, origin };
		}

		void Update(const Timer* pTimer)
		{
			updatedMatricesThisFrame = false;

			const float deltaTime = pTimer->GetElapsed();

			Vector3 movementMouse{};
			Vector3 movementKeyboard{};
			Vector3 rotation{};

			bool movDirty{};
			bool rotDirty{};
			float sprintModifier{ 1.0f };

			// Keyboard Input
			const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);

			if (pKeyboardState[SDL_SCANCODE_W] || pKeyboardState[SDL_SCANCODE_UP]) { movementKeyboard += forward; movDirty = true; }
			if (pKeyboardState[SDL_SCANCODE_S] || pKeyboardState[SDL_SCANCODE_DOWN]) { movementKeyboard -= forward; movDirty = true; }
			if (pKeyboardState[SDL_SCANCODE_D] || pKeyboardState[SDL_SCANCODE_RIGHT]) { movementKeyboard += right; movDirty = true; }
			if (pKeyboardState[SDL_SCANCODE_A] || pKeyboardState[SDL_SCANCODE_LEFT]) { movementKeyboard -= right; movDirty = true; }

			if (pKeyboardState[SDL_SCANCODE_SPACE] || pKeyboardState[SDL_SCANCODE_E]) { movementKeyboard += Vector3::UnitY; movDirty = true; }
			if (pKeyboardState[SDL_SCANCODE_LCTRL] || pKeyboardState[SDL_SCANCODE_Q]) { movementKeyboard -= Vector3::UnitY; movDirty = true; }
			if (pKeyboardState[SDL_SCANCODE_LSHIFT]) sprintModifier = 2.0f;

			// Mouse Input
			int mouseX{}, mouseY{};
			const uint32_t mouseState = SDL_GetRelativeMouseState(&mouseX, &mouseY);

			const bool rDown = mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT);
			const bool lDown = mouseState & SDL_BUTTON(SDL_BUTTON_LEFT);

			if (rDown && lDown) {
				movementMouse.y -= mouseY;
				movementMouse += right * static_cast<float>(mouseX);
				movDirty = true;
			}
			else if (rDown) {
				rotation.y += mouseX;
				rotation.x -= mouseY;
				rotDirty = true;
			}
			else if (lDown) {
				movementMouse -= forward * static_cast<float>(mouseY);
				rotation.y += mouseX;

				movDirty = true;
				rotDirty = true;
			}

			if (movDirty) {
				if (movementKeyboard.Normalize() > 0.0f) origin += movementKeyboard * movSpeed * deltaTime * sprintModifier;
				if (movementMouse.SqrMagnitude() > 0.0f) origin += movementMouse * panSpeed;
				viewDirty = true;
			}
			if (rotDirty && rotation.SqrMagnitude() > 0.0f) {
				const Vector3 scaledRotation = rotation * rotSpeed;

				constexpr float OFFSET = 0.05f;
				totalYaw += scaledRotation.x;
				totalYaw = std::clamp(totalYaw, -PI * 0.5f + OFFSET, PI * 0.5f - OFFSET);

				// Ensure always positive between 0 -> 6.28...
				totalPitch = std::fmodf(std::fmodf(scaledRotation.y + totalPitch, PI_2) + PI_2, PI_2);

				const Matrix fullRotMatrix = Matrix::CreateRotationX(totalYaw) * Matrix::CreateRotationY(totalPitch);
				forward = fullRotMatrix.TransformVector(Vector3::UnitZ);
				viewDirty = true;
			}

			//Update Matrices
			if (viewDirty) CalculateViewMatrix();
			if (projectDirty) CalculateProjectionMatrix();
		}
	};
}
