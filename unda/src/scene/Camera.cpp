#include "Camera.h"



unda::FPSCamera::FPSCamera(float _fov, float aRatio, float near, float far,
    const glm::vec3& pos, const glm::vec3& rot, 
    const glm::vec3& upDir, const glm::vec3& lookAt)
    : Camera(_fov, aRatio, near, far, pos, upDir, lookAt)
{
    Input::registerKeyCallBack(Key::Tab, [this]() { toggleMovement(); });
}

void unda::FPSCamera::handleInput()
{
    //if (Input::isKeyDown(Key::Tab))
    //    movementEnabled = !movementEnabled;

    if (!movementEnabled) return;
    glm::vec3 position = getPosition();
    float speed = cameraSpeed * (float)Time::getDeltaTime();
    if (Input::isKeyDown(Key::W)) position += speed * front;
    if (Input::isKeyDown(Key::S)) position -= speed * front;
    if (Input::isKeyDown(Key::A)) position -= glm::normalize(glm::cross(front, up)) * speed;
    if (Input::isKeyDown(Key::D)) position += glm::normalize(glm::cross(front, up)) * speed;

    auto [x, y] = Input::getMousePosition();
    float xOffset = ((float)x - lastX) * sensitivity;
    float yOffset = (lastY - (float)y) * sensitivity;
    lastX = (float)x;
    lastY = (float)y;
    yaw += xOffset;
    pitch += yOffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cosf(glm::radians(pitch)) * cosf(glm::radians(yaw));
    direction.z = cosf(glm::radians(pitch)) * sinf(glm::radians(yaw));
    direction.y = sinf(glm::radians(pitch));
    front = glm::normalize(direction);
    setPosition(position);
}

void unda::FPSCamera::toggleMovement()
{
    if (!Input::isKeyDown(Key::Tab)) movementEnabled = !movementEnabled;
    Input::lockCursor(movementEnabled);
}
