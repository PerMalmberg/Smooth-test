#include <driver/gpio.h>
#include <smooth/network/Wifi.h>
#include <smooth/Task.h>
#include <smooth/network/IPv4.h>
#include <smooth/network/IPv6.h>
#include <smooth/network/Socket.h>
#include <smooth/Application.h>
#include <smooth/ipc/Publisher.h>
#include <esp_system.h>
#include <smooth/network/ISendablePacket.h>
#include <smooth/network/IReceivablePacket.h>
#include <smooth/timer/Timer.h>
#include <esp_event.h>

#include "SSLTest.h"
#include "LedControl.h"

#include "wifi-creds.h"

using namespace smooth;
using namespace smooth::ipc;
using namespace smooth::network;
using namespace std::chrono;

class MyApp
        : public Application
{
    public:
        MyApp() : Application(tskIDLE_PRIORITY + 6, std::chrono::seconds(1))
        {
            gpio_pad_select_gpio(GPIO_NUM_5);
            gpio_set_direction(GPIO_NUM_5, GPIO_MODE_OUTPUT);
            gpio_set_level(GPIO_NUM_5, 0);
        }

        void message(const system_event_t& msg)
        {
            if (msg.event_id == SYSTEM_EVENT_STA_CONNECTED)
            {
                gpio_set_level(GPIO_NUM_5, 1);
            }
            else if (msg.event_id == SYSTEM_EVENT_STA_DISCONNECTED)
            {
                gpio_set_level(GPIO_NUM_5, 0);

            }

            Application::message(msg);
        }
};


extern "C" void app_main()
{
    // Create the application, it will run on the main task
    // so set an appropriate stack size on the config.
    MyApp app;

    Wifi& wifi = app.get_wifi();
    wifi.set_host_name("HAP-ESP32");
    wifi.set_ap_credentials(WIFI_SSID, WIFI_PASSWORD);
    wifi.set_auto_connect(true);

    LedControl led;
    led.start();

    SSLTest ssl_test;
    ssl_test.start();

    //app.set_system_log_level(ESP_LOG_ERROR);

    ESP_LOGV("Main", "Free heap: %u", esp_get_free_heap_size());

    // Start the application. Note that this function never returns.
    app.start();



}