#pragma once
#include "UI/UIComponent.hpp"

namespace IWXMVM::UI
{
	class DebugPanel : public UIComponent
	{
	public:
		DebugPanel()
		{
			Initialize();
		}
		~DebugPanel()
		{
			Release();
		}

		void Render() final;
		void Release() final;

	private:
		void Initialize() final;
	};
}