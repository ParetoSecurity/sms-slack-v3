#pragma once

namespace app
{

    class SystemManager_
    {
    private:
        SystemManager_() = default;
        void processSMSQueue();

    public:
        static SystemManager_ &getInstance();

        void setup();
        void loop();
    };

    extern SystemManager_ &manager;
}