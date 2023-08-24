#pragma once
#include "UI/UIComponent.hpp"

namespace IWXMVM::UI
{
	class CaptureMenu : public UIComponent
	{
	public:
		CaptureMenu()
		{
			Initialize();
		}
		~CaptureMenu()
		{
			Release();
		}

		void Render() final;
		void Release() final;

	private:
		void Initialize() final;
	};
}