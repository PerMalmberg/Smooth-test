#include <driver/gpio.h>
#include <smooth/core/network/Wifi.h>


#include "LedControl.h"
#include "MQTTTest.h"
#include <smooth/core/timer/PerfCount.h>

#include "wifi-creds.h"

using namespace smooth::core;
using namespace smooth::core::ipc;
using namespace smooth::core::network;
using namespace std::chrono;

static const std::string mqtt_broker = "192.168.10.247";

class MyApp
        : public Application
{
    public:
        MyApp() : Application(tskIDLE_PRIORITY + 1, std::chrono::seconds(1)),
                  mqtt()
        {
        }

        void init() override
        {
            Application::init();

            auto address = std::make_shared<smooth::core::network::IPv4>(mqtt_broker, 1883);
            mqtt.start(address);
        }

    private:
        MQTTTest mqtt;
};

extern "C" void app_main()
{
    // Create the application, it will run on the main task
    // so set an appropriate stack size in the config.
    MyApp app;


    Wifi& wifi = app.get_wifi();
    wifi.set_host_name("HAP-ESP32");
    wifi.set_ap_credentials(WIFI_SSID, WIFI_PASSWORD);
    wifi.set_auto_connect(true);
    app.set_system_log_level(ESP_LOG_ERROR);

/*
    LedControl led;
    led.start();
*/

    app.set_system_log_level(ESP_LOG_ERROR);

    ESP_LOGV("Main", "Free heap: %u", esp_get_free_heap_size());

    // Start the application. Note that this function never returns.
    app.start();


}