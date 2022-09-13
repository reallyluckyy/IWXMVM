#pragma once

namespace IWXMVM::IW3
{
	class DemoPlaybackPatch
	{
	public:
		static void Install();
		static void Uninstall();

		static bool IsInstalled();
	};
}
