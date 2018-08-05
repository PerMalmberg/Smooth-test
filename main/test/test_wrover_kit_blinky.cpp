#ifdef TEST_WROVER_KIT_RGB_BLINKY

#include "test_wrover_kit_blinky.h"
#include <smooth/core/task_priorities.h>

using namespace smooth;
using namespace smooth::core;
using namespace std::chrono;

TestApp::TestApp()
        : Application(APPLICATION_BASE_PRIO,
          milliseconds(300))
{
}

void TestApp::init()
{
    Application::init();
    Log::info("LED fun", Format("Lets blink some LEDs"));
}

void TestApp::tick()
{
    // Toggle LEDs to represent the current state value.
    g.set(static_cast<bool>(state & 1));
    r.set(static_cast<bool>(state & 2));
    b.set(static_cast<bool>(state & 4));

    state++;
}


#endif // TEST_WROVER_KIT_RGB_BLINKY