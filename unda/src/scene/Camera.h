#pragma once

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include "../rendering/RenderTools.h"
#include "../utils/Settings.h"
#include "../input/Input.h"
#include "../core/Time.h"
#include <iostream>



namespace unda {

	class Camera : public Transform {
	public:
		Camera(float _fov, float aspectRatio, float nearClippingPlane, float farClippingPlane, 
			const glm::vec3& pos = glm::vec3(0, 0, 0),
			const glm::vec3& upDirection = glm::vec3(0, 1, 0), const glm::vec3& lookAt = glm::vec3(0, 0, 0)) 
		{
			fov = _fov;
			aRatio = aspectRatio;
			near = nearClippingPlane;
			far = farClippingPlane;
			target = lookAt;
			up = upDirection;

		}
		Camera() {

		}

		const glm::mat4& getViewMatrix() { return viewMatrix; }
		const glm::mat4& getProjectionMatrix() { return projectionMatrix; }
		void setPosition(const glm::vec3& newPos) { unda::Transform::setPosition(newPos); update(); }
		void setRotation(const glm::vec3& newRot) { unda::Transform::setPosition(newRot); update(); }
		void setTarget(const glm::vec3& newTarget) { target = newTarget; update(); }
		inline const glm::vec3& getTarget() { return target; }

	protected:
		float fov = 90.0f, near = 0.1f, far = 100.0f, aRatio = (float)unda::windowWidth / (float)unda::windowHeight;

		glm::mat4 viewMatrix = glm::mat4(1.0f), projectionMatrix = glm::mat4(1.0f);

		glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f), up = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
		virtual void update() {
			glm::vec3 position = getPosition();
			viewMatrix = glm::lookAt(position, position + front, up);
			projectionMatrix = glm::perspective(glm::radians(fov), aRatio, near, far);
		}
	};

	class FPSCamera : public Camera {
	public:
		FPSCamera(float _fov, float aRatio, float near, float far,
			const glm::vec3& pos = glm::vec3(0, 0, 0), const glm::vec3& rot = glm::vec3(0, 0, 0),
			const glm::vec3& upDir = glm::vec3(0, 1, 0), const glm::vec3& lookAt = glm::vec3(0, 1, 0))
			: Camera(_fov, aRatio, near, far, pos, upDir, lookAt)
		{

		}
		void handleInput();

	private:
		float cameraSpeed = 3.0f, sensitivity = 0.05f;
		float pitch = 0.0f, yaw = 0.0f, lastX = (float)unda::windowWidth / 2.0f, lastY = (float)unda::windowHeight / 2.0f;
		bool firstInput = true;
	};
}