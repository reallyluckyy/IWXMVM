#pragma once
#include "UI/UIComponent.hpp"
#include <string>

namespace IWXMVM::UI
{
	class ConfigMenu : public UIComponent
	{
	public:
		ConfigMenu()
		{
			Initialize();
		}
		~ConfigMenu()
		{
			Release();
		}

		void Render() final;
		void Release() final;


	private:
		void Initialize() final;
		std::string LoadTextFromFile();
		static constexpr int TEXT_BUFFER_SIZE = 1.6e+7;
		char textBuffer[TEXT_BUFFER_SIZE] = "Load a config or paste it here!";
	};
}