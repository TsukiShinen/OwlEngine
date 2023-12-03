#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>


namespace Owl::Renderer::Vulkan
{
	class Camera
	{
	public:
		void SetOrthographicProjection(float pLeft, float pRight, float pTop, float pBottom, float pNear, float pFar);

		void SetPerspectiveProjection(float pFovY, float pAspect, float pNear, float pFar);

		void SetViewDirection(glm::vec3 pPosition, glm::vec3 pDirection, glm::vec3 pUp = glm::vec3{0.f, -1.f, 0.f});
		void SetViewTarget(glm::vec3 pPosition, glm::vec3 pTarget, glm::vec3 pUp = glm::vec3{0.f, -1.f, 0.f});
		void SetViewYxz(glm::vec3 pPosition, glm::vec3 pRotation);

		[[nodiscard]] const glm::mat4& GetProjection() const { return m_ProjectionMatrix; }
		[[nodiscard]] const glm::mat4& GetView() const { return m_ViewMatrix; }
		[[nodiscard]] const glm::mat4& GetInverseView() const { return m_InverseViewMatrix; }
		[[nodiscard]] const glm::vec3& GetPosition() const { return glm::vec3(m_InverseViewMatrix[3]); }

	private:
		glm::mat4 m_ProjectionMatrix{1.f};
		glm::mat4 m_ViewMatrix{1.0f};
		glm::mat4 m_InverseViewMatrix{1.0f};
	};
}
