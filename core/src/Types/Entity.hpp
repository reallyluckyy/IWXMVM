#pragma once

#include <glm/glm.hpp>

namespace IWXMVM::Types
{
    enum class EntityType
    {
        Unsupported,
        Player, 
        Corpse, 
        Item,
        Missile,
        Helicopter
    };

    struct Entity
    {
        int32_t id;
        EntityType type;
        int32_t clientNum; // associated client number
        bool isValid;

        std::string ToString()
        {
            auto EntityTypeToString = [](Types::EntityType type) -> std::string {
                switch (type)
                {
                    case EntityType::Unsupported:
                        return "Unsupported";
                    case EntityType::Player:
                        return "Player";
                    case EntityType::Corpse:
                        return "Corpse";
                    case EntityType::Item:
                        return "Item";
                    case EntityType::Missile:
                        return "Missile";
                    case EntityType::Helicopter:
                        return "Helicopter";
                }
                throw std::runtime_error("Unhandled entity type");
            };

            return EntityTypeToString(type) + std::format(" {0}", id);
        }
    };
}  // namespace IWXMVM::Types