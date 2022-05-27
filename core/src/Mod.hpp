#pragma once

namespace IWXMVM
{

	enum class Game : uint32_t {
		IW3
	};

	class Mod 
	{
	public:

		static void Initialize(Game game);
		static Game GetGame() { return currentGame; }

	private:
		static Game currentGame;
	};
}