#pragma once

#include "../rendering/RenderTools.h"
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include "../utils/Settings.h"


namespace unda {

	class Camera : public Transform {
	public:
		Camera(float _fov, float aRatio, float near, float far, const glm::vec3& pos = glm::vec3(0, 0, 0), const glm::vec3& rot = glm::vec3(0, 0, 0), const glm::vec3& upDir = glm::vec3(0, 1, 0), const glm::vec3& lookAt = glm::vec3(0, 1, 0)) {
			target = lookAt;
			upDirection = upDir;
			fov = _fov;
			aspectRatio = aRatio;
			nearClippingPlane = near;
			farClippingPlane = far;
			viewMatrix = glm::mat4(1.0f);
			projectionMatrix = glm::mat4(1.0f);
		}
		Camera() {
			target = glm::vec3(0.5f, 0.0f, 0.5f);
			upDirection = glm::vec3(0, 1, 0);
			fov = 90.0f;
			aspectRatio = (float)unda::windowHeight / (float)unda::windowWidth;
			nearClippingPlane = 0.5f;
			farClippingPlane = 90.0f;
			viewMatrix = glm::mat4(1.0f);
			projectionMatrix = glm::mat4(1.0f);
		}

		const glm::mat4& getViewMatrix() { return viewMatrix; }
		const glm::mat4& getProjectionMatrix() { return projectionMatrix; }
		void setPosition(const glm::vec3& newPos) override { setPosition(newPos); update(); }
		void setRotation(const glm::vec3& newRot) override { setPosition(newRot); update(); }
		void setTarget(const glm::vec3& newTarget) { target = newTarget; update(); }

	private:
		glm::vec3 target, upDirection;
		float fov, aspectRatio, nearClippingPlane, farClippingPlane;
		glm::mat4 viewMatrix, projectionMatrix;

		void update() {
			viewMatrix = glm::lookAt(getPosition(), target, upDirection);
			projectionMatrix = glm::perspective(glm::radians(fov), aspectRatio, nearClippingPlane, farClippingPlane);
		}
	};

}