#include <driver/gpio.h>
#include <smooth/core/network/Wifi.h>
#include <smooth/core/ipc/TaskEventQueue.h>
#include <smooth/application/network/mqtt/MqttClient.h>
#include <smooth/core/Application.h>
#include "esp_system.h"
#include "esp_log.h"

#undef write
#undef read

#include <sstream>

#include "wifi-creds.h"

using namespace smooth::core;
using namespace smooth::core::ipc;
using namespace smooth::core::network;
using namespace std::chrono;
using namespace smooth::application::network::mqtt;

static const std::string mqtt_broker = "192.168.10.247";

class MyApp
        : public Application,
          public IEventListener<MQTTData>
{
    public:
        MyApp() : Application(tskIDLE_PRIORITY + 1, std::chrono::seconds(5)),
                  mqtt_data("mqtt_data", 10, *this, *this),
                  mqtt("TestMQTT", std::chrono::seconds(30), 4096, tskIDLE_PRIORITY + 1, mqtt_data)
        {
        }

        void init() override
        {
            Application::init();

            mqtt.start();
            auto address = std::make_shared<smooth::core::network::IPv4>(mqtt_broker, 1883);
            mqtt.connect_to(address, true);
            mqtt.subscribe("SubTopic", QoS::AT_MOST_ONCE);
            mqtt.subscribe("Foo/Bar", QoS::EXACTLY_ONCE);
            mqtt.subscribe("Wildcard/#", QoS::AT_LEAST_ONCE);
            mqtt.unsubscribe("Foo");
        }

        void event(const MQTTData& event) override
        {
            std::stringstream ss;
            for (auto& b : event.second)
            {
                ss << static_cast<char>(b);
            }

            ESP_LOGV("Received", "%s: %s", event.first.c_str(), ss.str().c_str());
        }

        void tick() override
        {
            ESP_LOGV("Main", "Free heap: %u", esp_get_free_heap_size());

            mqtt.publish("TOPIC", "0", EXACTLY_ONCE, false);
            mqtt.publish("TOPIC1", "1", AT_MOST_ONCE, false);
            mqtt.publish("TOPIC2", "2", AT_LEAST_ONCE, false);
        }

    private:
        smooth::core::ipc::TaskEventQueue<MQTTData> mqtt_data;
        smooth::application::network::mqtt::MqttClient mqtt;
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

    // Start the application. Note that this function never returns.
    app.start();
}