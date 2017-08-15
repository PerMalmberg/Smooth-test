//
// Created by permal on 7/15/17.
//

#include "MQTTTest.h"
#include <smooth/application/network/mqtt/MQTTProtocolDefinitions.h>

#undef write
#undef read

#include <sstream>

using namespace smooth::application::network::mqtt;

MQTTTest::MQTTTest()
        : mqtt("TestMQTT", std::chrono::seconds(30), 4096, tskIDLE_PRIORITY + 1)
{
    time.start();
}

void MQTTTest::start(std::shared_ptr<smooth::core::network::InetAddress> address)
{
    mqtt.start();
    mqtt.connect_to(address, true);
    mqtt.subscribe("SubTopic");
    mqtt.subscribe("Foo/Bar");
    mqtt.subscribe("Wildcard/#");
}

void MQTTTest::disconnect()
{
    mqtt.disconnect();
}

void MQTTTest::publish_data()
{
    std::stringstream ss;
    ss << time.get_running_time().count();
    mqtt.publish("TOPIC", ss.str(), EXACTLY_ONCE, false);
    mqtt.publish("TOPIC1",ss.str(), AT_MOST_ONCE, false);
    mqtt.publish("TOPIC2", ss.str(), AT_LEAST_ONCE, false);
}

