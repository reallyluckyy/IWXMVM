#include "StdInclude.hpp"
#include "Hooks.hpp"

#include "Events.hpp"
#include "Utilities/HookManager.hpp"


namespace IWXMVM::IW3::Hooks
{

	// TODO: Turn this into a "EventCallbacks" "patch"

	typedef void(*RB_EndFrame_t)();
	RB_EndFrame_t RB_EndFrame;
	void __declspec(naked) RB_EndFrame_Hook()
	{
		__asm pushad

		Events::Invoke(EventType::OnFrame);
		
		__asm popad
		__asm jmp RB_EndFrame
	}

	typedef void(*CL_PlayDemo_t)();
	CL_PlayDemo_t CL_PlayDemo_f;
	void __declspec(naked) CL_PlayDemo_Hook()
	{
		__asm pushad

		Events::Invoke(EventType::OnDemoLoad);

		__asm popad
		__asm jmp CL_PlayDemo_f
	}

	void Install()
	{
		RB_EndFrame = HookManager::CreateHook<RB_EndFrame_t>(0x615460, RB_EndFrame_Hook, 6);
		CL_PlayDemo_f = HookManager::CreateHook<CL_PlayDemo_t>(0x4692D9, CL_PlayDemo_Hook, 9);
	}
}