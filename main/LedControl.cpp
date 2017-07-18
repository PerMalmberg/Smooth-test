//
// Created by permal on 7/18/17.
//

#include "LedControl.h"

using namespace std::chrono;
using namespace smooth::network;
using namespace smooth::ipc;
using namespace smooth;

LedControl::LedControl() : Task("LedControl", 8192, 5, milliseconds(10)),
                           txEmpty("txEmpty", 1, *this, *this),
                           data_available("data_available", 5, *this, *this),
                           connection_status("connection_status", 3, *this, *this),
                           timer_expired("timer_expired", 10, *this, *this),
                           tx(),
                           rx(),
                           s(tx, rx, txEmpty, data_available, connection_status),
                           network_timer("Foo", 1, timer_expired, true, std::chrono::seconds(2)),
                           steady_blink("stead_blink", 2, timer_expired, true, std::chrono::milliseconds(500))
{

}

void LedControl::init()
{
    gpio_pad_select_gpio(GPIO_NUM_5);
    gpio_set_direction(GPIO_NUM_5, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_NUM_5, 0);

    gpio_pad_select_gpio(GPIO_NUM_26);
    gpio_set_direction(GPIO_NUM_26, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_NUM_26, 0);

    network_timer.start();
    steady_blink.start();
    auto ip = std::make_shared<smooth::network::IPv4>("192.168.10.44", 5566);
    s.start(ip);
}

void LedControl::message(const DataAvailableEvent <TestPacket>& msg)
{
    TestPacket data;
    if (msg.get(data))
    {
        for (int i = 0; i < 5; ++i)
        {
            ESP_LOGV("D", "%c", data.get_data()[i]);
        }


        if (data.get_data()[0] == '1')
        {
            gpio_set_level(GPIO_NUM_5, 1);
        }
        else if (data.get_data()[0] == '0')
        {
            gpio_set_level(GPIO_NUM_5, 0);
        }
        else if (data.get_data()[0] == 'x')
        {
            TestPacket tp;
            tx.put(tp);
        }
        else if (data.get_data()[0] == 's')
        {
            stress = !stress;
        }
    }
}

void LedControl::message(const TransmitBufferEmptyEvent& msg)
{
    if (stress)
    {
        tx.put(TestPacket());
    }
}

void LedControl::message(const ConnectionStatusEvent& msg)
{

}

void LedControl::message(const timer::TimerExpiredEvent& msg)
{
    if (msg.get_timer() == &network_timer)
    {
        if (s.is_active())
        {
            tx.put(TestPacket());
        }
        else
        {
            s.restart();
        }
    }
    else if (msg.get_timer() == &steady_blink)
    {
        led_on = !led_on;
        gpio_set_level(GPIO_NUM_26, led_on ? 1 : 0);
    }
}