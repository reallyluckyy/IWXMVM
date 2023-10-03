#pragma once
#include "Camera.hpp"
#include "Types/CampathNode.hpp"

namespace IWXMVM::Components
{
    class DollyCamera : public Camera
    {
       public:
        DollyCamera()
        {
            this->mode = Camera::Mode::Dolly;
        }

        void Initialize() override;
        void Update() override;
    };
}  // namespace IWXMVM::Components