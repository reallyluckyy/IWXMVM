#include "StdInclude.hpp"
#include "Camera.hpp"

#include "Components/ShakeManager.hpp"
#include "Utilities/MathUtils.hpp"
#include "glm/vec3.hpp"

namespace IWXMVM::Components
{
    glm::vec3 Camera::GetFinalPosition() const
    {
        glm::vec3 shake = ShakeManager::Get().CalculatePositionShake();
        return position + positionOffset + shake;
    }

    glm::vec3 Camera::GetFinalRotation() const
    {
        glm::vec3 shake = ShakeManager::Get().CalculateRotationShake();
        return rotation + rotationOffset + shake;
    }

    bool Camera::IsModControlledCameraMode()
    {
        return mode != Mode::FirstPerson && mode != Mode::ThirdPerson;
    }

    glm::vec3 Camera::GetForwardVector()
    {
        return MathUtils::ForwardVectorFromAngles(this->rotation);
    }

    glm::vec3 Camera::GetRightVector()
    {
        return glm::normalize(glm::cross(GetForwardVector(), glm::vector3::up) * -1);
    }

    glm::vec3 Camera::GetUpVector()
    {
        return glm::normalize(glm::cross(GetForwardVector(), GetRightVector()));
    }

    void Camera::SetForwardVector(glm::vec3 forward)
    {
        this->rotation = MathUtils::AnglesFromForwardVector(forward);
    }
}  // namespace IWXMVM::Components