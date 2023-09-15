#pragma once
#include "Camera.hpp"
#include "Types/Marker.hpp"

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

	private:
		std::vector<Types::Marker> markers;
	};
}