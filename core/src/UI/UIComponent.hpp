#pragma once

#define REGISTER_UI_COMPONENT(x)

namespace IWXMVM::UI
{
	class UIComponent
	{
	public:
		UIComponent() = default;
		virtual ~UIComponent() = default;

		virtual void Initialize() = 0;
		virtual void Render() = 0;
		virtual void Release() = 0;

		ImVec2 GetPosition() const { return position; }
		void SetPosition(ImVec2 newPos) { position.x = std::floor(newPos.x); position.y = std::floor(newPos.y); }
		void SetPosition(float posX, float posY) { position.x = std::floor(posX); position.y = std::floor(posY); }
		void SetPositionX(float posX) { position.x = std::floor(posX); }
		void SetPositionY(float posY) { position.y = std::floor(posY); }

		ImVec2 GetSize() const { return size; }
		void SetSize(ImVec2 newSize) { size.x = std::floor(newSize.x); size.y = std::floor(newSize.y); }
		void SetSize(float sizeX, float sizeY) { size.x = std::floor(sizeX); size.y = std::floor(sizeY); }
		void SetSizeX(float sizeX) { size.x = std::floor(sizeX); }
		void SetSizeY(float sizeY) { size.y = std::floor(sizeY); }
	private:
		ImVec2 position = {};
		ImVec2 size = {};
	};
}