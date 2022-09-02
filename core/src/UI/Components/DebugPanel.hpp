#pragma once
#include "UI/UIComponent.hpp"

namespace IWXMVM::UI
{
	class DebugPanel : public UIComponent
	{
	public:
		void Initialize() override;
		void Render() override;
	};
}
