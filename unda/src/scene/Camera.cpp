#include "Camera.h"


void unda::FPSCamera::handleInput()
{
    auto [x, y] = Input::getMousePosition();
    if (firstInput)
    {
        lastX = x;
        lastY = x;
        firstInput = false;
    }
    glm::vec3 position = getPosition();
    float speed = cameraSpeed * (float)Time::getDeltaTime();
    if (Input::isKeyDown(Key::W)) position += speed * front;
    if (Input::isKeyDown(Key::S)) position -= speed * front;
    if (Input::isKeyDown(Key::A)) position -= glm::normalize(glm::cross(front, upDirection)) * speed;
    if (Input::isKeyDown(Key::D)) position += glm::normalize(glm::cross(front, upDirection)) * speed;
    setPosition(position);

    glm::vec3 target = getTarget();


    float xOffset = x - lastX;
    float yOffset = y - lastY;
    yaw += xOffset;
    pitch += yOffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    target.x = cosf(glm::radians(pitch)) * cosf(glm::radians(yaw));
    target.y = sinf(glm::radians(pitch));
    target.z = cosf(glm::radians(pitch)) * sinf(glm::radians(yaw));

    front = glm::normalize(-target);
    setTarget(target);
}

