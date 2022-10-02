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
	};
}
