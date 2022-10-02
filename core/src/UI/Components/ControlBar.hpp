#pragma once
#include "UI/UIComponent.hpp"

namespace IWXMVM::UI
{
	class ControlBar : public UIComponent
	{
	public:
		ControlBar()
		{
			Initialize();
		}
		~ControlBar()
		{
			Release();
		}

		void Render() final;
		void Release() final;

	private:
		void Initialize() final;
	};
}