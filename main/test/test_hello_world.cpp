#ifdef TEST_HELLO_WORLD

#include "test_hello_world.h"
#include <smooth/core/task_priorities.h>

using namespace smooth;
using namespace smooth::core;
using namespace smooth::core::logging;
using namespace std::chrono;

TestApp::TestApp()
        : Application(APPLICATION_BASE_PRIO,
          seconds(1))
{
}

void TestApp::init()
{
    Application::init();
    Log::info("HelloWorld", Format("Hello world initialized."));
}

void TestApp::tick()
{
    Log::info("Hello", Format("World"));
}


#endif // TEST_HELLO_WORLD