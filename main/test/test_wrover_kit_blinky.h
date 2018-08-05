#pragma once

#ifdef TEST_WROVER_KIT_RGB_BLINKY

#include <smooth/core/Application.h>
#include <smooth/core/ipc/IEventListener.h>
#include <smooth/core/ipc/TaskEventQueue.h>
#include <smooth/core/timer/Timer.h>
#include <smooth/core/io/Output.h>

class TestApp
        : public smooth::core::Application
{
    public:

        TestApp();

        void init() override;
        void tick() override;

    private:
        smooth::core::io::Output r{GPIO_NUM_0, true, false, false, false};
        smooth::core::io::Output g{GPIO_NUM_2, true, false, false, false};
        smooth::core::io::Output b{GPIO_NUM_4, true, false, false, false};
        uint8_t state = 0;

};

#endif // TEST_WROVER_KIT_RGB_BLINKY