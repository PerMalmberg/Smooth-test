

#include <test/test_queue.h>
#include <test/test_timer.h>
#include <test/test_network.h>
#include <test/test_rgb_led.h>
#include <test/test_json.h>

#ifdef ESP_PLATFORM
extern "C" void app_main()
{
    // Create the application, it will run on the main task
    // so set an appropriate stack size in the config.
    TestApp app;

    // Start the application. Note that this function never returns.
    app.start();
}

#else // END ESP_PLATFORM

int main()
{
    // Create the application, it will run on the main task
    // so set an appropriate stack size in the config.
    TestApp app;

    // Start the application. Note that this function never returns.
    app.start();

    return 0;
}

#endif