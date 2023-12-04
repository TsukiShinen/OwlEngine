#pragma once
#include <GLFW/glfw3.h>

#include "RotateSystem.h"
#include "../Components/KeyboardInputComponent.h"
#include "../Components/MovementComponent.h"

class MovementControllerSystem : public Owl::Ecs::System
{
public:
	void MoveInPlaneXZ(GLFWwindow* pWindow, float pDeltaTime)
	{
		for (const auto entity : m_Entities)
		{
			const auto& keyboardInputs = m_World->GetComponent<KeyboardInputComponent>(entity);
			auto& movement = m_World->GetComponent<MovementComponent>(entity);
			auto& transform = m_World->GetComponent<Owl::TransformComponent>(entity);

			glm::vec3 rotate{0};
			if (glfwGetKey(pWindow, keyboardInputs.LookRight) == GLFW_PRESS) rotate.y += 1.0f;
			if (glfwGetKey(pWindow, keyboardInputs.LookLeft) == GLFW_PRESS) rotate.y -= 1.0f;
			if (glfwGetKey(pWindow, keyboardInputs.LookUp) == GLFW_PRESS) rotate.x += 1.0f;
			if (glfwGetKey(pWindow, keyboardInputs.LookDown) == GLFW_PRESS) rotate.x -= 1.0f;

			if (dot(rotate, rotate) > std::numeric_limits<float>::epsilon())
				transform.Rotation += movement.RotationSpeed * pDeltaTime * normalize(rotate);

			transform.Rotation.x = glm::clamp(transform.Rotation.x, -1.5f, 1.5f);
			transform.Rotation.y = glm::mod(transform.Rotation.y, glm::two_pi<float>());

			float yaw = transform.Rotation.y;
			glm::vec3 forwardDirection{sin(yaw), 0.f, cos(yaw)};
			glm::vec3 rightDirection{forwardDirection.z, 0.f, -forwardDirection.x};
			glm::vec3 upDirection{0.f, -1.f, 0.f};

			glm::vec3 moveDirection{0.f};
			if (glfwGetKey(pWindow, keyboardInputs.MoveForward) == GLFW_PRESS) moveDirection += forwardDirection;
			if (glfwGetKey(pWindow, keyboardInputs.MoveBackward) == GLFW_PRESS) moveDirection -= forwardDirection;
			if (glfwGetKey(pWindow, keyboardInputs.MoveRight) == GLFW_PRESS) moveDirection += rightDirection;
			if (glfwGetKey(pWindow, keyboardInputs.MoveLeft) == GLFW_PRESS) moveDirection -= rightDirection;
			if (glfwGetKey(pWindow, keyboardInputs.MoveUp) == GLFW_PRESS) moveDirection += upDirection;
			if (glfwGetKey(pWindow, keyboardInputs.MoveDown) == GLFW_PRESS) moveDirection -= upDirection;

			if (dot(moveDirection, moveDirection) > std::numeric_limits<float>::epsilon())
				transform.Position += movement.MoveSpeed * pDeltaTime * normalize(moveDirection);
		}
	}
};
