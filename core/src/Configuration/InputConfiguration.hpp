#pragma once
#include "Configuration.hpp"

namespace IWXMVM
{
	enum Bind
	{
		DollyAddNode,
		DollyClearNodes,
		DollyPlayPath,
        FreeCameraBackward,
		FreeCameraDown,
		FreeCameraForward,
        FreeCameraLeft,
		FreeCameraReset,
        FreeCameraRight,
		FreeCameraUp,
		OrbitCameraMove,
		OrbitCameraReset,
		OrbitCameraRotate,
		PlaybackFaster,
		PlaybackSkipBackward,
		PlaybackSkipForward,
		PlaybackSlower,
		PlaybackToggle,

		Count,
	};

	const std::string_view NODE_ACTION = "action";
	const std::string_view NODE_KEY = "key";

	void BindsDeserialize(const nlohmann::json& j);
    void BindsSerialize(nlohmann::json& j);
	ImGuiKey BindToKey(Bind bind) noexcept;
    Bind BindFromString(std::string_view string);
    std::string_view BindToString(Bind bind) noexcept;
}  // namespace IWXMVM