#ifdef TEST_NETWORK

#include "test_network.h"

#ifdef ESP_PLATFORM
#include <smooth/core/network/Wifi.h>
#include "wifi-creds.h"
#endif

using namespace smooth;
using namespace smooth::core;
using namespace smooth::core::logging;
using namespace std::chrono;

TestApp::TestApp()
        : Application(10, seconds(10)),
          mqtt_data("mqtt_data", 10, *this, *this),
          client("Smooth", seconds(10), 8192, 10, mqtt_data)
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
    client.connect_to(std::make_shared<network::IPv4>("192.168.10.44", 1883), true);
}

void TestApp::event(const smooth::application::network::mqtt::MQTTData& event)
{

}


#endif // TEST_NETWORK