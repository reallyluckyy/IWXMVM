#pragma once
namespace IWXMVM::TaskbarList
{
    class TaskbarList
    {
        public:
            TaskbarList(HWND hwnd);
            void Initialize(HWND hwnd);
            void SetProgressState(TBPFLAG);
            void SetProgressValue(unsigned long long, unsigned long long);
            ~TaskbarList();

       private:
            ITaskbarList3* pTaskbarList;
            HWND taskbarHWND;
            TBPFLAG state;
    };

}