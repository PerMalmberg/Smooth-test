//
// Created by permal on 7/15/17.
//

#pragma once

#include <smooth/application/network/mqtt/MQTT.h>
#include <smooth/core/network/InetAddress.h>
#include <smooth/core/timer/PerfCount.h>

class MQTTTest
{
    public:
        MQTTTest();

        void start(std::shared_ptr<smooth::core::network::InetAddress> address);
        void disconnect();

        void publish_data();

    private:
        smooth::application::network::mqtt::MQTT mqtt;
        smooth::core::timer::PerfCount time{};
};
