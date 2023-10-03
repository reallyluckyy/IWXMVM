#include "StdInclude.hpp"
#include "Camera.hpp"

#include "Utilities/MathUtils.hpp"
#include "glm/vec3.hpp"

namespace IWXMVM::Components
{
    bool Camera::IsModControlledCameraMode()
    {
        return mode != Mode::FirstPerson && mode != Mode::ThirdPerson;
    }

    glm::vec3 Camera::GetForwardVector()
    {
        return MathUtils::ForwardVectorFromAngles(this->rotation[0], this->rotation[1], this->rotation[2]);
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