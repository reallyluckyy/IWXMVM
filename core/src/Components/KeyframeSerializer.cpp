#include "StdInclude.hpp"
#include "KeyframeSerializer.hpp"

#include "nlohmann/json.hpp"

#include "Utilities/PathUtils.hpp"
#include "KeyframeManager.hpp"
#include "Mod.hpp"

namespace IWXMVM::Components
{
    constexpr std::string_view NODE_GAME_NAME = "game";
    constexpr std::string_view NODE_DEMO_NAME = "demo";
    constexpr std::string_view NODE_PROPERTIES = "properties";
    constexpr std::string_view NODE_PROPERTY = "property";
    constexpr std::string_view NODE_TICK = "tick";
    constexpr std::string_view NODE_TYPE = "type";
    constexpr std::string_view NODE_VALUES = "values";
    constexpr std::string_view NODE_VALUE = "value";
    constexpr std::string_view NODE_KEYFRAMES = "keyframes";

    void KeyframeSerializer::Write(std::filesystem::path path)
    {
        using json = nlohmann::json;

        if (!std::filesystem::exists(path.parent_path()))
        {
            std::filesystem::create_directories(path.parent_path());
        }

        json rootNode;
        rootNode[NODE_GAME_NAME] = magic_enum::enum_name(Mod::GetGameInterface()->GetGame());
        rootNode[NODE_DEMO_NAME] = Mod::GetGameInterface()->GetDemoInfo().name;
        
        json properties = json::array();

        for (auto& [p, ks] : KeyframeManager::Get().GetKeyframes())
        {
            json propertyObject;
            propertyObject[NODE_PROPERTY] = magic_enum::enum_name(p.type);

            json keyframeList = json::array();
            for (auto& k : ks)
            {
                json keyframeObject;
                keyframeObject[NODE_TICK] = k.tick;

                json keyframeValueObject;
                keyframeValueObject[NODE_TYPE] = magic_enum::enum_name(p.valueType);

                json keyframeValuesArray;
                keyframeValuesArray = json::array();
                for (int i = 0; i < p.GetValueCount(); i++)
                {
                    keyframeValuesArray.push_back(k.value.GetByIndex(i));
                }
                keyframeValueObject[NODE_VALUES] = keyframeValuesArray;

                keyframeObject[NODE_VALUE] = keyframeValueObject;
                keyframeList.push_back(keyframeObject);
            }

            propertyObject[NODE_KEYFRAMES] = keyframeList;
            properties.push_back(propertyObject);
        }

        rootNode[NODE_PROPERTIES] = properties;

        std::ofstream keyframeFile(path);
        keyframeFile << rootNode.dump(4);
        keyframeFile.close();

        LOG_INFO("Wrote keyframes to {}", path.string());
    }

    Types::KeyframeValue ReadValueOfType(const Types::KeyframeValueType valueType, const nlohmann::json& values)
    {
        switch (valueType)
        {
            case Types::KeyframeValueType::FloatingPoint:
                return Types::KeyframeValue(values[0].get<float>());
            case Types::KeyframeValueType::Vector3:
                return Types::KeyframeValue(
                    glm::vec3(
                        values[0].get<float>(),
                        values[1].get<float>(), 
                        values[2].get<float>()
                    )
                );
            case Types::KeyframeValueType::CameraData:
                auto cameraData = Types::CameraData();
                cameraData.position = glm::vec3(values[0].get<float>(), values[1].get<float>(), values[2].get<float>());
                cameraData.rotation = glm::vec3(values[3].get<float>(), values[4].get<float>(), values[5].get<float>());
                cameraData.fov = values[6].get<float>();
                return Types::KeyframeValue(cameraData);
            default:
                LOG_ERROR("Value type {} is unhandled", magic_enum::enum_name(valueType));
                throw std::runtime_error("Encountered unhandled value type");
        }
    }

    void KeyframeSerializer::Read(std::filesystem::path path)
    {
        using json = nlohmann::json;

        LOG_INFO("Reading keyframes from {}", path.string());

        std::ifstream file(path);
        if (!file.is_open())
        {
            LOG_ERROR("Failed to read keyframe file");
        }

        try
        {
            json rootNode = json::parse(file);

            auto gameName = rootNode[NODE_GAME_NAME].get<std::string>();
            auto currentGameName = magic_enum::enum_name(Mod::GetGameInterface()->GetGame());
            if (gameName.compare(currentGameName) != 0)
            {
                LOG_WARN("Game of loaded keyframes file doesnt match current game!");
                LOG_WARN("Expected: {0}", gameName);
                LOG_WARN("Actual: {1}", currentGameName);
            }

            auto demoName = rootNode[NODE_DEMO_NAME].get<std::string>();
            auto currentDemoName = Mod::GetGameInterface()->GetDemoInfo().name;
            if (demoName.compare(currentDemoName) != 0)
            {
                LOG_WARN("Demo names dont match {0} vs {1}", demoName, currentDemoName);
                LOG_WARN("Expected: {0}", demoName);
                LOG_WARN("Actual: {1}", currentDemoName);
            }

            auto properties = rootNode[NODE_PROPERTIES];
            for (json::iterator propertyObject = properties.begin(); propertyObject != properties.end();
                 ++propertyObject)
            {
                auto propertyName = (*propertyObject)[NODE_PROPERTY].get<std::string>();

                auto propertyType = magic_enum::enum_cast<Types::KeyframeablePropertyType>(propertyName);
                if (!propertyType.has_value())
                {
                    LOG_ERROR("Unknown property \"{0}\"", propertyName);
                    throw std::runtime_error("Unknown property encountered");
                }
                auto property = Components::KeyframeManager::Get().GetProperty(propertyType.value());

                auto& keyframes = Components::KeyframeManager::Get().GetKeyframes(property);
                for (json::iterator keyframe = propertyObject->at(NODE_KEYFRAMES).begin();
                     keyframe != propertyObject->at(NODE_KEYFRAMES).end(); ++keyframe)
                {
                    auto tick = keyframe->at(NODE_TICK).get<uint32_t>();
                    auto valueNode = keyframe->at(NODE_VALUE);
                    auto valueTypeName = valueNode[NODE_TYPE].get<std::string>();
                    auto valueType = magic_enum::enum_cast<Types::KeyframeValueType>(valueTypeName);
                    if (!valueType.has_value())
                    {
                        LOG_ERROR("Unknown value type \"{0}\"", valueTypeName);
                        throw std::runtime_error("Unknown value type encountered");
                    }
                    Types::KeyframeValue value = ReadValueOfType(valueType.value(), valueNode[NODE_VALUES]);

                    keyframes.push_back(Types::Keyframe(property, tick, Types::KeyframeValue(value)));
                }
            }
        }
        catch (const std::exception& e)
        {
            LOG_ERROR("Failed to parse keyframe file ({})", e.what());
        }
    }
}