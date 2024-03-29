#pragma once
#include "Camera.hpp"

namespace IWXMVM::Components
{
    class OrbitCamera : public Camera
    {
       public:
        OrbitCamera()
        {
            this->mode = Camera::Mode::Orbit;
        }

        void Initialize() override;
        void Update() override;

        glm::vec3 GetOrigin() const noexcept
        {
            return orbitCameraOrigin;
        }

       private:
        void DrawOverlay();

        void DrawOrbitPoint();
        void DrawGrid();

        glm::vec3 orbitCameraOrigin{};
    };
}  // namespace IWXMVM::Components