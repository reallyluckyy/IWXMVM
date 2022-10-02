#pragma once

namespace IWXMVM
{
	enum class EventType
	{
		OnFrame,
		OnDemoLoad
	};

	namespace Events
	{
		void Invoke(EventType eventType);
		void RegisterListener(EventType eventType, std::function<void()> function);
	}
}