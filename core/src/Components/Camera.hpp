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

	private:
		Mode mode;
		vec3_t position;
		vec3_t rotation;
		float fov;


	public:
		Camera(Mode mode) : mode(mode), fov(90), position(), rotation() {}

		Mode GetMode() { return mode; }
		vec3_t& GetPosition() { return position; }
		vec3_t& GetRotation() { return rotation; }
		float& GetFov() { return fov; }

		bool IsModControlledCameraMode();
	};
}