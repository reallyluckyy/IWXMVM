#pragma once
#include "GameInterface.hpp"
#include "Components/CameraManager.hpp"
#include "Components/CampathManager.hpp"
#include "Components/KeyframeManager.hpp"
#include "Components/CaptureManager.hpp"

namespace IWXMVM
{
    class Mod
    {
       public:
        static void Initialize(GameInterface* gameInterface);

        static GameInterface* GetGameInterface()
        {
            return internalGameInterface;
        }

        static void RequestEject();

       private:
        static GameInterface* internalGameInterface;

        static std::atomic<bool> ejectRequested;
    };
}  // namespace IWXMVM