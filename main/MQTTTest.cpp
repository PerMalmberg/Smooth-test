//
// Created by permal on 7/15/17.
//

#include "MQTTTest.h"
#include <driver/gpio.h>

using namespace smooth::application::network::mqtt;

MQTTTest::MQTTTest()
        : mqtt("TestMQTT", std::chrono::seconds(15), 4096, tskIDLE_PRIORITY + 6)
{
}

void MQTTTest::start(std::shared_ptr<smooth::core::network::InetAddress> address, bool use_ssl)
{
    mqtt.start();
    mqtt.connect_to(address, true, use_ssl);
}

