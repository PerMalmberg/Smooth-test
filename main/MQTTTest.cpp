//
// Created by permal on 7/15/17.
//

#include "MQTTTest.h"
#include <smooth/application/network/mqtt/MQTTProtocolDefinitions.h>

using namespace smooth::application::network::mqtt;

MQTTTest::MQTTTest()
        : mqtt("TestMQTT", std::chrono::seconds(30), 4096, tskIDLE_PRIORITY + 1)
{
}

void MQTTTest::start(std::shared_ptr<smooth::core::network::InetAddress> address)
{
    mqtt.start();
    mqtt.connect_to(address, true);
    mqtt.publish("TOPIC", "Test", EXACTLY_ONCE, false);
}

void MQTTTest::disconnect()
{
    mqtt.disconnect();
}

