#ifdef TEST_WROVER_KIT_RGB_BLINKY_JTAG_DEBUGGING

#include "test_wrover_kit_blinky_jtag_debugging.h"
#include <smooth/core/task_priorities.h>

using namespace smooth;
using namespace smooth::core;
using namespace std::chrono;

TestApp::TestApp()
        : Application(APPLICATION_BASE_PRIO,
          seconds(1))
{
}

void TestApp::init()
{
    Application::init();
    Log::info("JTAG Debugging",
            Format("This app blinks the green and blue LEDs on the Wrover kit and specifically doesn't touch GPIO 0"));
}

void TestApp::tick()
{
    // Toggle LEDs to represent the current state value.
    g.set(static_cast<bool>(state & 1));
    b.set(static_cast<bool>(state & 2));

    state++;
}


#endif // TEST_WROVER_KIT_RGB_BLINKY_JTAG_DEBUGGING