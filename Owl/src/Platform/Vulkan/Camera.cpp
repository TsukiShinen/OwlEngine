#include "opch.h"
#include "Camera.h"

namespace Owl::Renderer::Vulkan
{
	void Camera::SetOrthographicProjection(const float pLeft, const float pRight, const float pTop, const float pBottom,
	                                       const float pNear, const float pFar)
	{
		m_ProjectionMatrix = glm::mat4{1.0f};
		m_ProjectionMatrix[0][0] = 2.f / (pRight - pLeft);
		m_ProjectionMatrix[1][1] = 2.f / (pBottom - pTop);
		m_ProjectionMatrix[2][2] = 1.f / (pFar - pNear);
		m_ProjectionMatrix[3][0] = -(pRight + pLeft) / (pRight - pLeft);
		m_ProjectionMatrix[3][1] = -(pBottom + pTop) / (pBottom - pTop);
		m_ProjectionMatrix[3][2] = -pNear / (pFar - pNear);
	}

	void Camera::SetPerspectiveProjection(const float pFovY, const float pAspect, const float pNear, const float pFar)
	{
		assert(glm::abs(pAspect - std::numeric_limits<float>::epsilon()) > 0.0f);
		const float tanHalfFovY = tan(pFovY / 2.f);
		m_ProjectionMatrix = glm::mat4{0.0f};
		m_ProjectionMatrix[0][0] = 1.f / (pAspect * tanHalfFovY);
		m_ProjectionMatrix[1][1] = 1.f / (tanHalfFovY);
		m_ProjectionMatrix[2][2] = pFar / (pFar - pNear);
		m_ProjectionMatrix[2][3] = 1.f;
		m_ProjectionMatrix[3][2] = -(pFar * pNear) / (pFar - pNear);
	}

	void Camera::SetViewDirection(const glm::vec3 pPosition, const glm::vec3 pDirection, const glm::vec3 pUp)
	{
		const glm::vec3 w{normalize(pDirection)};
		const glm::vec3 u{normalize(cross(w, pUp))};
		const glm::vec3 v{cross(w, u)};

		m_ViewMatrix = glm::mat4{1.f};
		m_ViewMatrix[0][0] = u.x;
		m_ViewMatrix[1][0] = u.y;
		m_ViewMatrix[2][0] = u.z;
		m_ViewMatrix[0][1] = v.x;
		m_ViewMatrix[1][1] = v.y;
		m_ViewMatrix[2][1] = v.z;
		m_ViewMatrix[0][2] = w.x;
		m_ViewMatrix[1][2] = w.y;
		m_ViewMatrix[2][2] = w.z;
		m_ViewMatrix[3][0] = -dot(u, pPosition);
		m_ViewMatrix[3][1] = -dot(v, pPosition);
		m_ViewMatrix[3][2] = -dot(w, pPosition);

		m_InverseViewMatrix = glm::mat4{1.f};
		m_InverseViewMatrix[0][0] = u.x;
		m_InverseViewMatrix[0][1] = u.y;
		m_InverseViewMatrix[0][2] = u.z;
		m_InverseViewMatrix[1][0] = v.x;
		m_InverseViewMatrix[1][1] = v.y;
		m_InverseViewMatrix[1][2] = v.z;
		m_InverseViewMatrix[2][0] = w.x;
		m_InverseViewMatrix[2][1] = w.y;
		m_InverseViewMatrix[2][2] = w.z;
		m_InverseViewMatrix[3][0] = pPosition.x;
		m_InverseViewMatrix[3][1] = pPosition.y;
		m_InverseViewMatrix[3][2] = pPosition.z;
	}

	void Camera::SetViewTarget(const glm::vec3 pPosition, const glm::vec3 pTarget, const glm::vec3 pUp)
	{
		SetViewDirection(pPosition, pTarget - pPosition, pUp);
	}

	void Camera::SetViewYxz(const glm::vec3 pPosition, const glm::vec3 pRotation)
	{
		const float c3 = glm::cos(pRotation.z);
		const float s3 = glm::sin(pRotation.z);
		const float c2 = glm::cos(pRotation.x);
		const float s2 = glm::sin(pRotation.x);
		const float c1 = glm::cos(pRotation.y);
		const float s1 = glm::sin(pRotation.y);
		const glm::vec3 u{(c1 * c3 + s1 * s2 * s3), (c2 * s3), (c1 * s2 * s3 - c3 * s1)};
		const glm::vec3 v{(c3 * s1 * s2 - c1 * s3), (c2 * c3), (c1 * c3 * s2 + s1 * s3)};
		const glm::vec3 w{(c2 * s1), (-s2), (c1 * c2)};
		m_ViewMatrix = glm::mat4{1.f};
		m_ViewMatrix[0][0] = u.x;
		m_ViewMatrix[1][0] = u.y;
		m_ViewMatrix[2][0] = u.z;
		m_ViewMatrix[0][1] = v.x;
		m_ViewMatrix[1][1] = v.y;
		m_ViewMatrix[2][1] = v.z;
		m_ViewMatrix[0][2] = w.x;
		m_ViewMatrix[1][2] = w.y;
		m_ViewMatrix[2][2] = w.z;
		m_ViewMatrix[3][0] = -dot(u, pPosition);
		m_ViewMatrix[3][1] = -dot(v, pPosition);
		m_ViewMatrix[3][2] = -dot(w, pPosition);

		m_InverseViewMatrix = glm::mat4{1.f};
		m_InverseViewMatrix[0][0] = u.x;
		m_InverseViewMatrix[0][1] = u.y;
		m_InverseViewMatrix[0][2] = u.z;
		m_InverseViewMatrix[1][0] = v.x;
		m_InverseViewMatrix[1][1] = v.y;
		m_InverseViewMatrix[1][2] = v.z;
		m_InverseViewMatrix[2][0] = w.x;
		m_InverseViewMatrix[2][1] = w.y;
		m_InverseViewMatrix[2][2] = w.z;
		m_InverseViewMatrix[3][0] = pPosition.x;
		m_InverseViewMatrix[3][1] = pPosition.y;
		m_InverseViewMatrix[3][2] = pPosition.z;
	}
}
