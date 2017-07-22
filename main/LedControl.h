//
// Created by permal on 7/18/17.
//

#pragma once

#include <driver/gpio.h>
#include <esp_system.h>
#include <smooth/core/Task.h>
#include <smooth/core/network/IPv4.h>
#include <smooth/core/network/Socket.h>
#include <smooth/core/Application.h>
#include <smooth/core/ipc/Publisher.h>
#include <smooth/core/ipc/IEventListener.h>
#include <smooth/core/network/TransmitBufferEmptyEvent.h>
#include <smooth/core/network/ConnectionStatusEvent.h>
#include <smooth/core/network/DataAvailableEvent.h>
#include <smooth/core/network/ISendablePacket.h>
#include <smooth/core/network/IReceivablePacket.h>
#include <smooth/core/timer/Timer.h>
#include <smooth/core/timer/TimerExpiredEvent.h>

class TestPacket
        : public smooth::core::network::ISendablePacket, public smooth::core::network::IReceivablePacket
{
    public:
        TestPacket()
        {
            std::fill(std::begin(buff), std::end(buff), '.');
            *std::begin(buff) = '<';
            *(std::end(buff)-1) = '>';
        }

        int get_wanted_amount() override
        {
            return sizeof(buff) - data_count;
        }

        void data_received(int length) override
        {
            data_count += length;
            ESP_LOGV("data_count", "%d", data_count);
            if (data_count == sizeof(buff))
            {
                complete = true;
            }

            ESP_LOGV("complete", "%d", complete);
        }

        bool is_complete() override
        {
            return complete;
        }

        const uint8_t* get_data() override
        {
            return buff;
        }

        uint8_t* get_write_pos() override
        {
            return buff + data_count;
        }

        bool is_error() override
        {
            return data_count > sizeof(buff);
        }

        int get_send_length() override
        {
            return sizeof(buff);
        }

    private:
        int data_count = 0;
        bool complete = false;
        uint8_t buff[5]{};
};

class LedControl
        : public smooth::core::Task,
          public smooth::core::ipc::IEventListener<smooth::core::network::DataAvailableEvent<TestPacket>>,
          public smooth::core::ipc::IEventListener<smooth::core::network::TransmitBufferEmptyEvent>,
          public smooth::core::ipc::IEventListener<smooth::core::network::ConnectionStatusEvent>,
          public smooth::core::ipc::IEventListener<smooth::core::timer::TimerExpiredEvent>
{
    public:

        LedControl();


        void init() override;

        void message(const smooth::core::network::DataAvailableEvent<TestPacket>& msg) override;

        void message(const smooth::core::network::TransmitBufferEmptyEvent& msg) override;

        void message(const smooth::core::network::ConnectionStatusEvent& msg) override;

        void message(const smooth::core::timer::TimerExpiredEvent& msg) override;


    private:
        smooth::core::ipc::TaskEventQueue<smooth::core::network::TransmitBufferEmptyEvent> txEmpty;
        smooth::core::ipc::TaskEventQueue<smooth::core::network::DataAvailableEvent<TestPacket>> data_available;
        smooth::core::ipc::TaskEventQueue<smooth::core::network::ConnectionStatusEvent> connection_status;
        smooth::core::ipc::TaskEventQueue<smooth::core::timer::TimerExpiredEvent> timer_expired;
        smooth::core::network::PacketSendBuffer<TestPacket, 1> tx;
        smooth::core::network::PacketReceiveBuffer<TestPacket, 5> rx;
        smooth::core::network::Socket<TestPacket> s;
        smooth::core::timer::Timer network_timer;
        smooth::core::timer::Timer steady_blink;
        bool stress = false;
        bool led_on = false;
};


