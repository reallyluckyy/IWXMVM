#include "StdInclude.hpp"
#include "DemoParser.hpp"

#include "Mod.hpp"
#include "Structures.hpp"
#include "Utilities/PathUtils.hpp"

namespace IWXMVM::IW3::DemoParser
{
    uint32_t demoStartTick;
    uint32_t demoEndTick;

    std::pair<int32_t, int32_t> GetDemoTickRange()
    {
        return std::make_pair(demoStartTick, demoEndTick);
    }

    enum class DemoMessageType : uint8_t
    {
        NetworkPacket = 0,
        ClientArchive = 1,
        CoD4XProtocolHeader = 2
    };

    void SkipBytes(std::ifstream& file, const int size)
    {
        auto pFilestream = file.tellg();
        pFilestream += size;

        file.seekg(pFilestream, std::ios::beg);
    }

    void ReadDemoArchives(std::ifstream& file, std::vector<clientArchiveData_t>& archives)
    {
        clientArchiveData_t archive;
        file.read(reinterpret_cast<char*>(&archive), sizeof(clientArchiveData_t));

        if (archives.empty() || archive.serverTime > archives.back().serverTime)
        {
            archives.emplace_back(archive);
        }
    }

    void Run()
    {
        std::ifstream file(Mod::GetGameInterface()->GetDemoInfo().path, std::ios::binary);
        if (!file.is_open())
        {
            throw std::exception("failed to open demo file");
        }

        std::vector<clientArchiveData_t> archives;

        while (true)
        {
            char messageType;
            file.read(&messageType, 1);

            if (file.eof())
                break;

            switch (messageType)
            {
                case (uint8_t)DemoMessageType::NetworkPacket:
                {
                    int messageSize = -2;

                    SkipBytes(file, 4);
                    file.read(reinterpret_cast<char*>(&messageSize), 4);
                    SkipBytes(file, 4);

                    if (file.eof() || messageSize == -1)
                    {
                        break;
                    }

                    SkipBytes(file, messageSize - 4);
                    continue;
                }
                case (uint8_t)DemoMessageType::ClientArchive:
                    ReadDemoArchives(file, archives);
                    continue;
                case (uint8_t)DemoMessageType::CoD4XProtocolHeader:
                    SkipBytes(file, 16);
                    continue;
                default:
                    LOG_DEBUG("Encountered unhandled demo message type {0}", messageType);
                    break;
            }
        }

        if (archives.size() > 256)
        {
            // some of the first 256 archives are outdated (cod4)
            demoStartTick = archives[0].serverTime;
            demoEndTick = archives.back().serverTime + 500;

            LOG_DEBUG("Determined demo bounds as {0} and {1}", demoStartTick, demoEndTick);
        }
        else
        {
            demoStartTick = 0;
            demoEndTick = 0;

            LOG_ERROR("Could not determine demo length due to lack of 256 client archives (found {0})",
                      archives.size());
        }
    }
}  // namespace IWXMVM::IW3::DemoParser
