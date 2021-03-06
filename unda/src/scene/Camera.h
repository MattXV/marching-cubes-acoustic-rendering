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
			update();
		}
		Camera() = default;

		const glm::mat4& getViewMatrix() { return viewMatrix; }
		const glm::mat4& getProjectionMatrix() { return projectionMatrix; }
		void setPosition(const glm::vec3& newPos) { unda::Transform::setPosition(newPos); update(); }
		void setRotation(const glm::vec3& newRot) { unda::Transform::setPosition(newRot); update(); }
		void setTarget(const glm::vec3& newTarget) { target = newTarget; update(); }
		void setFront(const glm::vec3& newFront) { front = newFront; update(); }
		inline const glm::vec3& getTarget() { return target; }

		virtual void handleInput() {}
		virtual void update() {
			glm::vec3 position = Transform::getPosition();
			viewMatrix = glm::lookAt(position, position + front, up);
			projectionMatrix = glm::perspective(glm::radians(fov), aRatio, near, far);
		}
	protected:
		float fov = 90.0f, near = 0.1f, far = 100.0f, aRatio = (float)unda::windowWidth / (float)unda::windowHeight;
		glm::mat4 viewMatrix = glm::mat4(1.0f), projectionMatrix = glm::mat4(1.0f);
		glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f), up = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
	};

	class FPSCamera : public Camera {
	public:
		FPSCamera(float _fov, float aRatio, float near, float far,
			const glm::vec3& pos = glm::vec3(0, 0, 0), const glm::vec3& rot = glm::vec3(0, 0, 0),
			const glm::vec3& upDir = glm::vec3(0, 1, 0), const glm::vec3& lookAt = glm::vec3(0, 1, 0));
		void toggleMovement();
		void handleInput() override;
	private:
		float cameraSpeed = 3.0f, sensitivity = 0.05f;
		float pitch = 0.0f, yaw = 0.0f, lastX = (float)unda::windowWidth / 2.0f, lastY = (float)unda::windowHeight / 2.0f;
		bool firstInput = true, movementEnabled = false;
	};

	class OrthoCamera : public Camera {
	public:
		OrthoCamera(float _width, float _height, float _zNear, float _zFar) :
			width(_width),
			height(_height),
			zNear(_zNear),
			zFar(_zFar)
		{
			Transform::setPosition(glm::vec3(0));
			front = glm::vec3(0.0f, 0.0f, -1.0f);
			up = glm::vec3(0.0f, 1.0f, 0.0f);
			Input::registerKeyCallBack(Key::F, [this]() {  if (!Input::isKeyDown(Key::Tab)) movementEnabled = !movementEnabled; Input::lockCursor(movementEnabled); });

		}
		~OrthoCamera() = default;
		void handleInput() override;
		inline void update() override {
			glm::vec3 position = Transform::getPosition();
			viewMatrix = glm::lookAt(position, glm::normalize(position + front), up);
			projectionMatrix = glm::ortho<float>(-1.0f, 1.0f, -1.0, 1.0f, zNear, zFar);
		}

	private:	
		float width = 0.0f, height = 0.0f, zNear = 0.0f, zFar = 0.0f;
		float cameraSpeed = 3.0f, sensitivity = 0.005f;
		float pitch = 0.0f, yaw = -90.0f, lastX = (float)unda::windowWidth / 2.0f, lastY = (float)unda::windowHeight / 2.0f;
		bool firstInput = true, movementEnabled = false;
	};
}