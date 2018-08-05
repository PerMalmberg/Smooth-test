#pragma once

#ifdef TEST_HELLO_WORLD

#include <smooth/core/Application.h>
#include <smooth/core/ipc/IEventListener.h>
#include <smooth/core/ipc/TaskEventQueue.h>
#include <smooth/core/timer/Timer.h>

class TestApp
        : public smooth::core::Application
{
    public:

        TestApp();

        void init() override;
        void tick() override;
};

#endif // TEST_HELLO_WORLD