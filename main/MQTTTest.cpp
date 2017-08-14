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
    mqtt.publish("123456", "12345", AT_MOST_ONCE, false);
    mqtt.publish("TOPIC2", "Test2", AT_LEAST_ONCE, false);
    mqtt.subscribe("SubTopic");
    mqtt.subscribe("Foo/Bar");
    mqtt.subscribe("Wildcard/#");
}

void MQTTTest::disconnect()
{
    mqtt.disconnect();
}

