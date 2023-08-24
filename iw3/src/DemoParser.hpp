#pragma once

namespace IWXMVM::IW3::DemoParser
{
	struct clientArchiveData_t
	{
		int archiveIndex;
		float origin[3];
		float velocity[3];
		int movementDir;
		int bobCycle;
		int serverTime;
		float viewAngles[3];
	};

	void Run();

	extern uint32_t demoStartTick;
	extern uint32_t demoEndTick;
}