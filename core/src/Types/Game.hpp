#pragma once

namespace IWXMVM::Types
{
    enum class Game
    {
        None,
        IW3
    };

    static inline std::string_view ToString(Game game)
    {
        switch (game)
        {
            case Game::IW3:
                return "IW3";
            case Game::None:
            default:
                return "Unknown Game";
        }
    }
}  // namespace IWXMVM::Types