//
// Created by permal on 7/15/17.
//

#pragma once

#include <smooth/application/network/mqtt/MQTT.h>
#include <smooth/core/network/InetAddress.h>

class MQTTTest
{
    public:
        MQTTTest();

        void start(std::shared_ptr<smooth::core::network::InetAddress> address, bool use_ssl);

    private:
        smooth::application::network::mqtt::MQTT mqtt;
};
