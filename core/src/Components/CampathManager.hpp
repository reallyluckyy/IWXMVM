#pragma once

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

       private:
        CampathManager()
        {
        }
    };
}  // namespace IWXMVM::Components