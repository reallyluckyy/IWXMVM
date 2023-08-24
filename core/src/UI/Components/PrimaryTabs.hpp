#pragma once
#include "UI/UIComponent.hpp"

namespace IWXMVM::UI
{
	enum class Tab {
		CFG,
		DEMOS,
		CAPTURE,
		DEBUG,
	};

	class PrimaryTabs : public UIComponent
	{
	public:
		void Initialize() final;
		void Render() final;
		void Release() final;
	};
}