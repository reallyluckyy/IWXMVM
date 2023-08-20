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
		Vector3 position;
		Vector3 rotation;
		float fov;


	public:
		Camera(Mode mode) : mode(mode), fov(90), position(), rotation() {}

		Mode GetMode() { return mode; }
		Vector3& GetPosition() { return position; }
		Vector3& GetRotation() { return rotation; }
		float& GetFov() { return fov; }
		Vector3 GetForwardVector();
		void SetForwardVector(Vector3 forward);
		Vector3 GetRightVector();

		bool IsModControlledCameraMode();
	};
}