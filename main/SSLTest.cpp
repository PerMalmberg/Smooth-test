//
// Created by permal on 7/15/17.
//

#include "SSLTest.h"
#include <driver/gpio.h>

using namespace smooth::core::network;

SSLTest::SSLTest()
        : Task("SSLTest", 8192, 5, std::chrono::milliseconds(10)),
          txEmpty("txEmpty", 1, *this, *this),
          data_available("data_available", 20, *this, *this),
          connection_status("connection_status", 3, *this, *this),
          tx(),
          rx(),
          s(tx, rx, txEmpty, data_available, connection_status),
          timer_expired("timer_expired", 2, *this, *this),
          timer("Timer", 0, timer_expired, false, std::chrono::seconds(5))
{
}

void SSLTest::message(const DataAvailableEvent<HTTPPacket>& msg)
{
    HTTPPacket data;
    if (msg.get(data))
    {
        ESP_LOGV("SSLTest", "%s", data.to_string().c_str());
    }
}

void SSLTest::message(const TransmitBufferEmptyEvent& msg)
{
    ESP_LOGV("SSP", "Packet sent");
}

void SSLTest::message(const ConnectionStatusEvent& msg)
{
    if ( msg.is_connected())
    {
        HTTPPacket sp("GET / HTTP/1.1\r\n"
                              "Connection: close"
                              "\r\n\r\n");
        tx.put(sp);
    }
    else
    {
        timer.reset();
    }

    gpio_set_level(GPIO_NUM_25, msg.is_connected());
}

void SSLTest::message(const smooth::core::timer::TimerExpiredEvent& msg)
{
    if( !s.is_active() )
    {
        auto ip = std::make_shared<smooth::core::network::IPv4>("172.217.18.142", 443);
        s.start(ip);
    }
}

void SSLTest::init()
{
    gpio_pad_select_gpio(GPIO_NUM_25);
    gpio_set_direction(GPIO_NUM_25, GPIO_MODE_OUTPUT);
    timer.start();
}
