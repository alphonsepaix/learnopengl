#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>

#include <memory>
#include <unordered_map>

constexpr auto INITIAL_POS = glm::vec3(0.0f);
constexpr auto INITIAL_SPEED = 7.0f;

class Camera {
public:
    enum class Direction {
        Forward, Backward, Left, Right, Up, Down
    };

    explicit Camera(const glm::vec3 &position = INITIAL_POS,
                    float speed = INITIAL_SPEED);

    virtual ~Camera() = default;

    [[nodiscard]] const glm::mat4 &lookAt() const {
        return m_lookAt;
    }

    [[nodiscard]] const glm::vec3 &getPosition() const {
        return m_position;
    }

    [[nodiscard]] const glm::vec3 &getFront() const {
        return m_front;
    }

    virtual void widgets();

    virtual void move(const Direction &direction, float deltaTime);

    virtual void mouseUpdate(float xOffset, float yOffset);

    virtual void copy(Camera *camera);

protected:
    virtual void update();

    void baseWidgets();

    glm::vec3 m_position;
    glm::vec3 m_direction{};
    glm::vec3 m_front{};
    glm::vec3 m_up{};
    glm::vec3 m_right{};
    glm::mat4 m_lookAt{}; // transform any vector to the camera coordinate system (view space)

    float m_speed;
    float m_pitch; // rotation around x
    float m_yaw; // rotation around y
    float m_sensitivity;

    bool m_invertYAxis;
};

class CameraFps final : public Camera {
public:
    explicit CameraFps(const glm::vec3 &position = INITIAL_POS,
                       float speed = INITIAL_SPEED);

    void move(const Direction &direction, float deltaTime) override;
};

class CameraLock final : public Camera {
public:
    explicit CameraLock(const glm::vec3 &position = INITIAL_POS,
                        const glm::vec3 &target = INITIAL_POS + glm::vec3(0.0f, 0.0f, -2.0f),
                        float speed = INITIAL_SPEED);

    void move(const Direction &direction, float deltaTime) override;

    void mouseUpdate(float xOffset, float yOffset) override {
    }

    const glm::vec3 &getTarget(glm::vec3 &target) const {
        return m_target;
    }

    void setTarget(const glm::vec3 &target);

    void widgets() override;

    void copy(Camera *camera) override;

private:
    void update() override;

    glm::vec3 m_target;
};

class CameraManager {
public:
    enum class Type {
        Free, FPS, Lock
    };

    CameraManager(const CameraManager &) = delete;

    CameraManager &operator=(const CameraManager &) = delete;

    static CameraManager *getInstance() {
        static CameraManager instance;
        return &instance;
    };

    [[nodiscard]] Camera *getActiveCamera() const {
        return m_activeCamera;
    }

    void setActiveCamera(Type camera);

    float getFov() const { return m_fov; }

    void updateFov(float fov);

    void widgets();

private:
    CameraManager();

    static int getCameraIndex(Type type);

    float m_fov;
    Type m_activeCameraType;
    Camera *m_activeCamera;
    std::unordered_map<Type, std::unique_ptr<Camera> > m_cameras;
};

#endif
