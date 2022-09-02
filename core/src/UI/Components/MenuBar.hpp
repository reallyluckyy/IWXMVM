#pragma once
#include "UI/UIComponent.hpp"

namespace IWXMVM::UI
{
	class MenuBar : public UIComponent
	{
	public:
		void Initialize() override;
		void Render() override;
	};
}
