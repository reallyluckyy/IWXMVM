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
		glm::vec3 position;
		glm::vec3 rotation;
		float fov;


	public:
		Camera(Mode mode) : mode(mode), fov(90), position(), rotation() {}

		Mode GetMode() { return mode; }
		glm::vec3& GetPosition() { return position; }
		glm::vec3& GetRotation() { return rotation; }
		float& GetFov() { return fov; }
		glm::vec3 GetForwardVector();
		void SetForwardVector(glm::vec3 forward);
		glm::vec3 GetRightVector();

		bool IsModControlledCameraMode();
	};
}