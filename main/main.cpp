#include <driver/gpio.h>
#include <smooth/core/network/Wifi.h>


#include "SSLTest.h"
#include "LedControl.h"
#include "MQTTTest.h"

#include "wifi-creds.h"

using namespace smooth::core;
using namespace smooth::core::ipc;
using namespace smooth::core::network;
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
    app.set_system_log_level(ESP_LOG_ERROR);

    /*LedControl led;
    led.start();

    SSLTest ssl_test;
    ssl_test.start();
*/

    MQTTTest mqtt;
    auto address = std::make_shared<smooth::core::network::IPv4>("192.168.10.247", 1883);
    mqtt.start( address, false );


    //app.set_system_log_level(ESP_LOG_ERROR);

    ESP_LOGV("Main", "Free heap: %u", esp_get_free_heap_size());

    // Start the application. Note that this function never returns.
    app.start();



}