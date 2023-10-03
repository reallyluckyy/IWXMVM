#pragma once
#include "Types/Marker.hpp"

namespace IWXMVM::Components
{
    class CampathManager
    {
       public:
        static CampathManager& Get()
        {
            static CampathManager instance;
            return instance;
        }

        CampathManager(CampathManager const&) = delete;
        void operator=(CampathManager const&) = delete;

        void Initialize();
        void Update();

        const std::vector<Types::Marker>& GetMarkers() const
        {
            return markers;
        }
        void AddMarker(Types::Marker marker);

       private:
        CampathManager()
        {
        }

        // Vector storing campath markers in order of ticks
        std::vector<Types::Marker> markers;
    };
}  // namespace IWXMVM::Components