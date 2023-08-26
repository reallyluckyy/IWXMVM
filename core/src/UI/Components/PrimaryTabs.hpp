#pragma once
#include "UI/UIComponent.hpp"

namespace IWXMVM::UI
{
	enum class Tab {
		Demos,
		Visuals,
		Camera,
		Record
	};

	class PrimaryTabs : public UIComponent
	{
	public:
		void Initialize() final;
		void Render() final;
		void Release() final;
	private:
		void RenderSelectedTab();
	};
}