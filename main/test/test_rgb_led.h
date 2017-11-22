#pragma once

#if defined(TEST_RGB_LED) && defined(ESP_PLATFORM)

#include <smooth/core/Application.h>
#include <smooth/core/ipc/TaskEventQueue.h>
#include <smooth/application/rgb_led/RGBLed.h>
#include <smooth/application/rgb_led/RGBLedTiming.h>
#include <smooth/core/io/Output.h>

class TestApp
        : public smooth::core::Application
{
    public:

        TestApp();

        void init() override;
        void tick() override;

    private:
        smooth::application::rgb_led::RGBLed rgb;
        smooth::core::io::Output rgb_level_shift;

};

#endif // end defined(TEST_RGB_LED) && defined(ESP_PLATFORM)