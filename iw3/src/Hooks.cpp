#include "StdInclude.hpp"
#include "Hooks.hpp"

#include "Structures.hpp"
#include "Events.hpp"
#include "Utilities/HookManager.hpp"
#include "IW3Interface.hpp"
#include "Mod.hpp"

#include <UI/UIManager.cpp>

namespace IWXMVM::IW3::Hooks
{
	// TODO: Turn this into a "EventCallbacks" "patch"

	typedef HRESULT(__stdcall* EndScene_t)(IDirect3DDevice9* pDevice);
	EndScene_t EndScene;

	HRESULT __stdcall D3D_EndScene_Hook(IDirect3DDevice9* pDevice)
	{
		Events::Invoke(EventType::OnFrame);

		return EndScene(pDevice);
	}

	typedef HRESULT(__stdcall* Reset_t)(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters);
	Reset_t	Reset;

	HRESULT __stdcall D3D_Reset_Hook(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters)
	{
		for (const auto& component : UI::UIManager::uiComponents) 
		{
			component->Release();
		}

		ImGui_ImplDX9_InvalidateDeviceObjects();
		HRESULT hr = Reset(pDevice, pPresentationParameters);
		ImGui_ImplDX9_CreateDeviceObjects();

		return hr;
	}

	/*
	void CL_CGameRendering_Internal()
	{	
		// run every frame
	}

	typedef void(*CL_CGameRendering_t)();
	CL_CGameRendering_t CL_CGameRendering;
	void __declspec(naked) CL_CGameRendering_Hook()
	{
		__asm pushad

		CL_CGameRendering_Internal();

		__asm
		{
			popad
			jmp CL_CGameRendering
		}
	}
	*/

	void* ptrCL_PlayDemo_f = nullptr;
	void* ptrCL_ReplayDemo_f = nullptr;
	void* ptrCL_Vid_Restart_f = nullptr;

	Structures::cmd_function_t** cmd_functions = (Structures::cmd_function_t**)0x1410B3C;
	Structures::cmd_function_t** sv_cmd_functions = (Structures::cmd_function_t**)0x14099DC;

	void Cmd_ModifyServerCommand(const char* cmd_name, void* function, void*& oldFunction)
	{
		for (auto cmd = *sv_cmd_functions; cmd; cmd = cmd->next)
		{
			if (!strcmp(cmd_name, cmd->name) && function != nullptr) 
			{
				if (function != nullptr) 
				{
					oldFunction = (void*)cmd->function;
					cmd->function = function;

					return;
				}
			}
		}
	}

	void Cmd_ModifyCommand(const char* cmd_name, void* function, void*& oldFunction)
	{
		for (auto cmd = *cmd_functions; cmd; cmd = cmd->next)
		{
			if (!strcmp(cmd_name, cmd->name) && function != nullptr)
			{
				if (function != nullptr)
				{
					oldFunction = (void*)cmd->function;
					cmd->function = function;

					return;
				}
			}
		}
	}

	// TODO: doesn't support fullpath demos [/demo "C:\Path\demo.dm_1" fullpath] yet!
	void CL_PlayDemo_Hook()
	{
		for (auto cmd = *sv_cmd_functions; cmd; cmd = cmd->next)
		{
			if (!strcmp(cmd->name, "demo") && ptrCL_PlayDemo_f != nullptr) 
			{
				reinterpret_cast<uintptr_t(*)()>(ptrCL_PlayDemo_f)();

				Events::Invoke(EventType::OnDemoLoad);
				return;
			} 
		}
	}

	// TODO: replayDemo is not supported yet because the old demo path needs to be stored!
	void CL_ReplayDemo_Hook()
	{
		for (auto cmd = *cmd_functions; cmd; cmd = cmd->next) 
		{
			if (!strcmp(cmd->name, "replayDemo") && ptrCL_ReplayDemo_f != nullptr) 
			{
				//reinterpret_cast<uintptr_t(*)()>(ptrCL_ReplayDemo_f)();

				//Events::Invoke(EventType::OnDemoLoad); 
				return;
			}
		}
	}

	void CL_Vid_Restart_Hook()
	{
		for (auto cmd = *cmd_functions; cmd; cmd = cmd->next) 
		{
			if (!strcmp(cmd->name, "vid_restart") && ptrCL_Vid_Restart_f != nullptr)
			{
				if (UI::UIManager::SignalImGuiRestart()) 
					reinterpret_cast<uintptr_t(*)()>(ptrCL_Vid_Restart_f)();
				
				return;
			}
		}
	}

	auto GeneratePattern = [](const std::size_t length) 
	{
		// the size of the pattern is always 1000 to avoid using too much resources
		std::vector<char> arr(1000);
	
		// ensuring that onescnt > 0 && onescnt <= 1000 so that the game always advances at least 1ms
		const std::size_t onescnt = (1000.0 / (length / 1000.0) < 1) ? 1 : (1000.0 / (length / 1000.0) > 1000) ? 1000 : 1000.0 / (length / 1000.0);
		const std::size_t len = arr.size();
		const std::size_t zeroscnt = len - onescnt;
		std::size_t ones = 1;
		std::size_t zeros = 1;

		for (size_t i = 0; i < len; ++i) 
		{
			if (ones * zeroscnt < zeros * onescnt) 
			{
				++ones;
				arr[i] = 1;
			} 
			else 
			{
				++zeros;
				arr[i] = 0;
			}
		}

		return arr;
	};

	void SV_Frame_Internal(int& msec)
	{
		static int lastState = 0;
		static int lastTime = 0;
		static int callIndex = 0;
		static std::vector<char> pattern;

		// always return 0 msec when pausing demo playback
		if (Mod::GetGameInterface()->IsDemoPlaybackPaused())
		{
			msec = 0;
			return;
		}

		static constexpr int sizeFrameTimeArray = 32;
		const int* frameTimeIndex = reinterpret_cast<int*>(0x7437A0);
		const std::optional<Dvar> timescale = Mod::GetGameInterface()->GetDvar("timescale");

		// we can use the original msec value when its value is greater than 1, and/or when timescale is equal or greater than 1.0
		// we also check if the game has generated at least 32 frames since starting
		if (timescale.value().value->floating_point >= 1.0f || msec > 1 || *frameTimeIndex < sizeFrameTimeArray)
		{
			return;
		}

		const std::optional<Dvar> com_maxfps = Mod::GetGameInterface()->GetDvar("com_maxfps");
		const int* com_frameTime = reinterpret_cast<int*>(0x1476EFC);
		const int* com_frameTimes = reinterpret_cast<int*>(0x743720);
		
		// below we're going to generate a pattern of interleaved 0s and 1s based on frame times
		// we generate a new pattern each second, or whenever timescale or com_maxfps changes

		if (lastState != (com_maxfps.value().value->int32 | timescale.value().value->int32))
		{
			// timescale or com_maxfps was changed
			lastState = com_maxfps.value().value->int32 | timescale.value().value->int32;

			if (com_maxfps.value().value->int32 != 0 && com_maxfps.value().value->int32 <= sizeFrameTimeArray)
			{
				// com_maxfps > 0 && <= 32, so it takes one second or more for the all the frame times to be updated
				// since we generate a new pattern every second, we want to delay this until all old frame times are overwritten
				// therefore we increase lastTime, so that it's ahead of the game's time (com_frameTime)
				lastTime = *com_frameTime + (sizeFrameTimeArray / com_maxfps.value().value->int32) * 1000;
			} 
			else
			{
				// we increase lastTime by a bit to ensure all old frame times are overwritten before generating a pattern based on frame times
				lastTime = *com_frameTime + 100;
			}
			
			// generate a temporary pattern purely based on com_maxfps and timescale, and not the actual fps, since the current frame times may be outdated
			pattern = GeneratePattern(com_maxfps.value().value->int32 / timescale.value().value->floating_point);
		}
		else if (*com_frameTime - 1000 >= lastTime)
		{
			// update lastTime
			lastTime = *com_frameTime;

			// generate a new pattern every second
			// we do this by getting the average frame time of the last 32 frames
			// this function is called once per frame, so we calculate the number of calls it takes to advance 1000 ms

			const double averageFrameTime = std::accumulate(com_frameTimes, com_frameTimes + sizeFrameTimeArray, 0) / static_cast<double>(sizeFrameTimeArray);
			const double totalCalls = (1000 / averageFrameTime) / timescale.value().value->floating_point;

			// example: 500 fps, 2.0ms average frametime, 0.01 timescale
			// (1000 / 2) / 0.01 = 50000
			// we need to generate a pattern where 1000 * 1ms and 49000 * 0ms are interleaved
			pattern = GeneratePattern(totalCalls);
		} 
			
		if (pattern.size() == 1000) 
		{
			// advance (1ms) or pause(0ms) based on the pattern
			msec = pattern[callIndex++ % 1000];
		}
	}

	void __declspec(naked) SV_Frame_Hook()
	{
		static int msec;
		static int msec_tmp;

		__asm
		{
			pop ecx
			pushad
			mov msec, eax
			mov msec_tmp, ebx
		}

		if (msec == msec_tmp) // only true for CoD4X
		{
			SV_Frame_Internal(msec);
			__asm
			{
				popad
				mov ebx, msec
				ret
			}
		}
		else
		{
			SV_Frame_Internal(msec);
			__asm
			{
				popad
				mov eax, msec
				ret
			}
		}
	}

	void Install(IDirect3DDevice9* device)
	{
		void** vTable = *reinterpret_cast<void***>(device);

		std::size_t resetBytes = 5;
		std::size_t endSceneBytes = 7;

		if (*(uint8_t*)vTable[16] != 0x8B || *(uint8_t*)vTable[42] != 0x6A)
		{
			resetBytes = 6;
			endSceneBytes = 11;

			LOG_WARN("Different byte structure detected when installing D3D hooks");
		}

		Reset = (Reset_t)HookManager::CreateHook((std::uintptr_t)vTable[16], (std::uintptr_t)&D3D_Reset_Hook, resetBytes, true);
		EndScene = (EndScene_t)HookManager::CreateHook((std::uintptr_t)vTable[42], (std::uintptr_t)&D3D_EndScene_Hook, endSceneBytes, true);

		HookManager::CreateHook(0x53366E, (std::uintptr_t)&SV_Frame_Hook, 5, false);
		//CL_CGameRendering = (CL_CGameRendering_t)HookManager::CreateHook(0x474DA0, (std::uintptr_t)&CL_CGameRendering_Hook, 7, true);

		Cmd_ModifyCommand("replayDemo", CL_ReplayDemo_Hook, ptrCL_ReplayDemo_f);
		Cmd_ModifyServerCommand("demo", CL_PlayDemo_Hook, ptrCL_PlayDemo_f);
		Cmd_ModifyServerCommand("vid_restart", CL_Vid_Restart_Hook, ptrCL_Vid_Restart_f);

		if (ptrCL_Vid_Restart_f == nullptr)
			Cmd_ModifyCommand("vid_restart", CL_Vid_Restart_Hook, ptrCL_Vid_Restart_f); // CoD4X
	}
}
