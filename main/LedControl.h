//
// Created by permal on 7/18/17.
//

#pragma once

#include <driver/gpio.h>
#include <esp_system.h>
#include <smooth/Task.h>
#include <smooth/network/IPv4.h>
#include <smooth/network/Socket.h>
#include <smooth/Application.h>
#include <smooth/ipc/Publisher.h>
#include <smooth/ipc/IEventListener.h>
#include <smooth/network/TransmitBufferEmptyEvent.h>
#include <smooth/network/ConnectionStatusEvent.h>
#include <smooth/network/DataAvailableEvent.h>
#include <smooth/network/ISendablePacket.h>
#include <smooth/network/IReceivablePacket.h>
#include <smooth/timer/Timer.h>
#include <smooth/timer/TimerExpiredEvent.h>

class TestPacket
        : public smooth::network::ISendablePacket, public smooth::network::IReceivablePacket
{
    public:
        TestPacket()
        {
            memset(buff, '.', sizeof(buff));
            buff[0] = '<';
            buff[sizeof(buff) - 1] = '>';
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
        : public smooth::Task,
          public smooth::ipc::IEventListener<smooth::network::DataAvailableEvent<TestPacket>>,
          public smooth::ipc::IEventListener<smooth::network::TransmitBufferEmptyEvent>,
          public smooth::ipc::IEventListener<smooth::network::ConnectionStatusEvent>,
          public smooth::ipc::IEventListener<smooth::timer::TimerExpiredEvent>
{
    public:

        LedControl();


        void init() override;

        void message(const smooth::network::DataAvailableEvent<TestPacket>& msg) override;

        void message(const smooth::network::TransmitBufferEmptyEvent& msg) override;

        void message(const smooth::network::ConnectionStatusEvent& msg) override;

        void message(const smooth::timer::TimerExpiredEvent& msg) override;


    private:
        smooth::ipc::TaskEventQueue<smooth::network::TransmitBufferEmptyEvent> txEmpty;
        smooth::ipc::TaskEventQueue<smooth::network::DataAvailableEvent<TestPacket>> data_available;
        smooth::ipc::TaskEventQueue<smooth::network::ConnectionStatusEvent> connection_status;
        smooth::ipc::TaskEventQueue<smooth::timer::TimerExpiredEvent> timer_expired;
        smooth::network::PacketSendBuffer<TestPacket, 1> tx;
        smooth::network::PacketReceiveBuffer<TestPacket, 5> rx;
        smooth::network::Socket<TestPacket> s;
        smooth::timer::Timer network_timer;
        smooth::timer::Timer steady_blink;
        bool stress = false;
        bool led_on = false;
};


