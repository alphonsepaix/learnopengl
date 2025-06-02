#define DBG_MACRO_NO_WARNING
#include <dbg.h>
#include <fmt/format.h>
#include <imgui.h>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Camera.h"

#include <algorithm>
#include <array>
#include <utility>

constexpr float MIN_FOV = 1.0f;
constexpr float MAX_FOV = 70.0f;

Camera::Camera(const glm::vec3 &position, const float speed): m_position{position},
                                                              m_speed{speed},
                                                              m_pitch{0.0f},
                                                              m_yaw{-90.f},
                                                              m_sensitivity{0.1f},
                                                              m_invertYAxis{true} {
    Camera::update();
}


void Camera::widgets() {
    baseWidgets();
    ImGui::Checkbox("Invert y axis", &m_invertYAxis);
}

void Camera::move(const Direction &direction, const float deltaTime) {
    glm::vec3 delta;
    switch (direction) {
        case Direction::Forward:
            delta = m_front;
            break;
        case Direction::Backward:
            delta = -m_front;
            break;
        case Direction::Left:
            delta = -glm::normalize(glm::cross(m_front, m_up));
            break;
        case Direction::Right:
            delta = glm::normalize(glm::cross(m_front, m_up));
            break;
        case Direction::Up:
            delta = glm::vec3(0.0f, 1.0f, 0.0f);
            break;
        case Direction::Down:
            delta = glm::vec3(0.0f, -1.0f, 0.0f);
            break;
    }
    m_position += delta * m_speed * deltaTime;
    update();
}

void Camera::mouseUpdate(const float xOffset, float yOffset) {
    if (m_invertYAxis)
        yOffset = -yOffset;
    m_yaw += xOffset * m_sensitivity;
    m_pitch += yOffset * m_sensitivity;
    if (m_pitch > 89.0f)
        m_pitch = 89.0f;
    if (m_pitch < -89.0f)
        m_pitch = -89.0f;
    update();
}

void Camera::copy(Camera *camera) {
    m_position = camera->m_position;
    m_direction = camera->m_direction;
    m_front = camera->m_front;
    m_up = camera->m_up;
    m_right = camera->m_right;
    m_lookAt = camera->m_lookAt;

    m_speed = camera->m_speed;
    m_pitch = camera->m_pitch;
    m_yaw = camera->m_yaw;
    m_sensitivity = camera->m_sensitivity;

    m_invertYAxis = camera->m_invertYAxis;
}

void Camera::update() {
    m_front.x = glm::cos(glm::radians(m_yaw)) * glm::cos(glm::radians(m_pitch));
    m_front.y = glm::sin(glm::radians(m_pitch));
    m_front.z = glm::sin(glm::radians(m_yaw)) * glm::cos(glm::radians(m_pitch));
    m_direction = -m_front;
    static constexpr auto upWorld = glm::vec3(0.0f, 1.0f, 0.0f);
    m_right = glm::normalize(glm::cross(upWorld, m_direction));
    m_up = glm::normalize(glm::cross(m_direction, m_right));
    m_lookAt = glm::lookAt(m_position, m_position + m_front, m_up);
}

void Camera::baseWidgets() {
    const auto position = fmt::format("Position: ({}, {}, {})", m_position.x, m_position.y, m_position.z);
    ImGui::Text(position.c_str());
    ImGui::SliderFloat("Speed", &m_speed, 0.1f, 10.0f);
}

CameraFps::CameraFps(const glm::vec3 &position, float speed): Camera{position, speed} {
}

void CameraFps::move(const Direction &direction, const float deltaTime) {
    glm::vec3 delta;
    switch (direction) {
        case Direction::Up:
        case Direction::Down:
            return;
        case Direction::Forward:
            delta = m_front;
            break;
        case Direction::Backward:
            delta = -m_front;
            break;
        case Direction::Left:
            delta = -glm::normalize(glm::cross(m_front, m_up));
            break;
        case Direction::Right:
            delta = glm::normalize(glm::cross(m_front, m_up));
            break;
    }
    delta.y = 0.0;
    m_position += glm::normalize(delta) * m_speed * deltaTime;
    update();
}

CameraLock::CameraLock(const glm::vec3 &position,
                       const glm::vec3 &target,
                       float speed): Camera(position, speed), m_target{target} {
    update();
}

void CameraLock::move(const Direction &direction, const float deltaTime) {
    const auto previous = m_position;
    Camera::move(direction, deltaTime);
    if (glm::length(m_position - m_target) < 1.0f)
        m_position = previous;
    // Update pitch and yaw angles based on new camera direction.
    m_pitch = glm::degrees(glm::asin(m_front.y));
    m_yaw = glm::degrees(glm::atan(m_front.z, m_front.x));
}

void CameraLock::setTarget(const glm::vec3 &target) {
    if (target == m_target)
        return;
    m_target = target;
    update();
}

void CameraLock::widgets() {
    baseWidgets();
    auto target = m_target;
    ImGui::SliderFloat3("Target", glm::value_ptr(target), -10.0f, 10.0f);
    setTarget(target);
}

void CameraLock::copy(Camera *camera) {
    Camera::copy(camera);
    m_target = m_position + 10.0f * m_front;
}

void CameraLock::update() {
    m_direction = glm::normalize(m_position - m_target);
    m_front = -m_direction;
    static constexpr auto upWorld = glm::vec3(0.0f, 1.0f, 0.0f);
    m_right = glm::normalize(glm::cross(upWorld, m_direction));
    m_up = glm::normalize(glm::cross(m_direction, m_right));
    m_lookAt = glm::lookAt(m_position, m_position + m_front, m_up);
}

void CameraManager::setActiveCamera(const Type camera) {
    m_activeCameraType = camera;
    if (const auto it = m_cameras.find(camera); it != m_cameras.end()) {
        const auto previous = m_activeCamera;
        m_activeCamera = it->second.get();
        if (previous && previous != m_activeCamera) {
            m_activeCamera->copy(previous);
        }
    }
}

void CameraManager::updateFov(float yOffset) {
    m_fov = std::clamp(m_fov - yOffset, MIN_FOV, MAX_FOV);
}

void CameraManager::widgets() {
    if (ImGui::CollapsingHeader("Camera")) {
        constexpr std::array cameraTypes = {"Free", "FPS", "Locked"};
        int cameraIndex = getCameraIndex(m_activeCameraType);
        ImGui::Combo("Camera", &cameraIndex, cameraTypes.data(),
                     cameraTypes.size());
        switch (cameraIndex) {
            case 0:
                setActiveCamera(Type::Free);
                break;
            case 1:
                setActiveCamera(Type::FPS);
                break;
            case 2:
                setActiveCamera(Type::Lock);
                break;
            default:
                break;
        }
        getActiveCamera()->widgets(); // camera widget
        ImGui::Text("FOV: %f", m_fov);
    }
}

CameraManager::CameraManager(): m_fov{MAX_FOV}, m_activeCameraType{Type::Free} {
    constexpr auto initialPos = glm::vec3(0.0f, 0.0f, 3.0f);
    m_cameras[Type::Free] = std::make_unique<Camera>(initialPos);
    m_cameras[Type::FPS] = std::make_unique<CameraFps>(initialPos);
    m_cameras[Type::Lock] = std::make_unique<CameraLock>(initialPos);
    m_activeCamera = m_cameras.at(Type::Free).get();
}

int CameraManager::getCameraIndex(const Type type) {
    switch (type) {
        case Type::Free:
            return 0;
        case Type::FPS:
            return 1;
        case Type::Lock:
            return 2;
        default:
            std::unreachable();
    }
}
