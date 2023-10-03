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
    };
}  // namespace IWXMVM::Components