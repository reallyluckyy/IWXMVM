#pragma once

namespace IWXMVM::Types
{
enum class GameState
{
    MainMenu,
    InGame,
    InDemo
};

static inline std::string_view ToString(GameState state)
{
    switch (state)
    {
        case GameState::MainMenu:
            return "Main Menu";
        case GameState::InDemo:
            return "Playing Demo";
        case GameState::InGame:
            return "In Game";
        default:
            return "Unknown Game";
    }
}
}  // namespace IWXMVM::Types