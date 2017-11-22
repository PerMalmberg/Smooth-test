#pragma once

#ifdef TEST_NETWORK

#include <smooth/core/Application.h>
#include <smooth/core/ipc/IEventListener.h>
#include <smooth/core/ipc/TaskEventQueue.h>
#include <smooth/application/network/mqtt/MqttClient.h>
#include <smooth/core/io/Output.h>


class TestApp
        : public smooth::core::Application,
        smooth::core::ipc::IEventListener<smooth::application::network::mqtt::MQTTData>
{
    public:

        TestApp();

        void init() override;
        void event(const smooth::application::network::mqtt::MQTTData& event) override;

        void tick() override;

    private:
        smooth::core::ipc::TaskEventQueue<smooth::application::network::mqtt::MQTTData> mqtt_data;
        smooth::application::network::mqtt::MqttClient client;
        smooth::core::io::Output led;

};

#endif // TEST_NETWORK