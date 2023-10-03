#pragma once

namespace IWXMVM::Components
{
class Camera
{
   public:
    enum class Mode
    {
        FirstPerson,
        ThirdPerson,
        Free,
        Orbit,
        Dolly,
        Bone,
        Count
    };

   protected:
    Mode mode;
    glm::vec3 position{};
    glm::vec3 rotation{};
    float fov = 90;

   public:
    virtual void Initialize() = 0;
    virtual void Update() = 0;

    Mode GetMode()
    {
        return mode;
    }
    glm::vec3& GetPosition()
    {
        return position;
    }
    glm::vec3& GetRotation()
    {
        return rotation;
    }
    float& GetFov()
    {
        return fov;
    }

    glm::vec3 GetForwardVector();
    glm::vec3 GetRightVector();
    glm::vec3 GetUpVector();

    void SetForwardVector(glm::vec3 forward);

    bool IsModControlledCameraMode();
};
}  // namespace IWXMVM::Components