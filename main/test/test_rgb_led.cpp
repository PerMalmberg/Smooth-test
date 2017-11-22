#if defined(TEST_RGB_LED) && defined(ESP_PLATFORM)

#include "test_rgb_led.h"
#include <smooth/core/task_priorities.h>


using namespace smooth;
using namespace smooth::core;
using namespace smooth::core::logging;
using namespace std::chrono;


TestApp::TestApp()
        : Application(APPLICATION_BASE_PRIO, milliseconds(200)),
          rgb(RMT_CHANNEL_0, GPIO_NUM_2, 5, smooth::application::rgb_led::WS2812B()),
          rgb_level_shift(GPIO_NUM_5, true, false, true)
{
}

void TestApp::init()
{
    Application::init();
    rgb_level_shift.set();
}


void TestApp::tick()
{
    static uint16_t i = 0;

    for(auto ix = 0; ix < 5; ++ix)
    {
        rgb.set_pixel(ix, 0, 0, 0);
    }

    rgb.set_pixel(i, 55, 0, 0);
    rgb.apply();

    if(++i == 5)
    {
        i = 0;
    }
}


#endif // end defined(TEST_RGB_LED) && defined(ESP_PLATFORM)