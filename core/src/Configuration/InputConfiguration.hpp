#pragma once
#include "Configuration.hpp"

namespace IWXMVM 
{
	using json = nlohmann::json;

	typedef std::variant<ImGuiKey, ImGuiMouseButton_> Key;

	class InputConfiguration
	{
	public:

		static InputConfiguration& Get()
		{
			static InputConfiguration instance;
			return instance;
		}

		InputConfiguration(InputConfiguration const&) = delete;
		void operator=(InputConfiguration const&) = delete;

		const std::unordered_map<std::string, Key>& GetKeyBinds() const { return keyBinds; }

		Key GetKeyBind(std::string_view actionName) const;
		void AddKeyBind(std::string_view actionName, Key key);

		void Serialize(json& j);
		void Deserialize(const json& j);

		void LoadDefault();
	private:
		InputConfiguration() {}

		std::unordered_map<std::string, Key> keyBinds;

		const std::string_view NODE_ACTION = "action";
		const std::string_view NODE_KEY = "key";
	};
}