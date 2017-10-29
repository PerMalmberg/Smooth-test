#ifdef TEST_NETWORK

#include "test_network.h"
#include <smooth/core/task_priorities.h>

#ifdef ESP_PLATFORM
#include <smooth/core/network/Wifi.h>
#include "wifi-creds.h"
#endif

using namespace smooth;
using namespace smooth::core;
using namespace smooth::core::logging;
using namespace std::chrono;
using namespace smooth::application::network::mqtt;

const char* broker = "192.168.10.245";
#ifdef ESP_PLATFORM
const char* client_id = "ESP32";
#else
const char* client_id = "Linux";
#endif

TestApp::TestApp()
        : Application(APPLICATION_BASE_PRIO, seconds(10)),
          mqtt_data("mqtt_data", 10, *this, *this),
          client(client_id, seconds(10), 8192, 10, mqtt_data)
{
}

void TestApp::init()
{
    Application::init();
#ifdef ESP_PLATFORM
    network::Wifi& wifi = get_wifi();
    wifi.set_host_name("Smooth-ESP");
    wifi.set_auto_connect(true);
    wifi.set_ap_credentials(WIFI_SSID, WIFI_PASSWORD);
    wifi.connect_to_ap();
#endif

    client.connect_to(std::make_shared<network::IPv4>(broker, 1883), true);
#ifdef ESP_PLATFORM
    client.subscribe("To:ESP32", QoS::EXACTLY_ONCE);
    client.publish("To:Linux", "Message to Linux", QoS::EXACTLY_ONCE, false);
#else
    client.subscribe("To:Linux", QoS::EXACTLY_ONCE);
    client.publish("To:ESP32", "Message to ESP32", QoS::EXACTLY_ONCE, false);
#endif
}

void TestApp::event(const smooth::application::network::mqtt::MQTTData& event)
{
    Log::info("Rec", Format("T:{1}, M:{2}", Str(event.first), Array(event.second, true)));
    if(event.first.find("ESP32") != std::string::npos)
    {
        client.publish("To:Linux", "Message to Linux", QoS::EXACTLY_ONCE, false);
    }
    else if(event.first.find("Linux") != std::string::npos)
    {
        client.publish("To:ESP32", "Message to ESP32", QoS::EXACTLY_ONCE, false);
    }
}

void TestApp::tick()
{
#ifdef ESP_PLATFORM
    client.publish("To:Linux", "Message to Linux", QoS::EXACTLY_ONCE, false);
#else
    client.publish("To:ESP32", "Message to ESP32", QoS::EXACTLY_ONCE, false);
#endif
}


#endif // TEST_NETWORK