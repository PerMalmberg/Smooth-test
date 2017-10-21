

#include <test/test_queue.h>


extern "C" void app_main()
{
    // Create the application, it will run on the main task
    // so set an appropriate stack size in the config.
    TestApp app;

    // Start the application. Note that this function never returns.
    app.start();
}
