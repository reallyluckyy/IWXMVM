#pragma once
#include "UI/UIComponent.hpp"

namespace IWXMVM::UI
{
	class CampathMenu : public UIComponent
	{
	public:
		void Initialize() final;
		void Render() final;
		void Release() final;
	private:
		void RenderHotkeyOverlay();
	};
}