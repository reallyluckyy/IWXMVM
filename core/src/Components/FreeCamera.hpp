#pragma once
#include "Camera.hpp"

namespace IWXMVM::Components
{
    class FreeCamera : public Camera
    {
       public:
        FreeCamera()
        {
            this->mode = Camera::Mode::Free;
        }

        void Initialize() override;
        void Update() override;

        static void HandleFreecamInput(glm::vec3& position, glm::vec3& rotation, float& fov, glm::vec3 forward,
                                       glm::vec3 right);
    };
}  // namespace IWXMVM::Components