//
// Created by permal on 7/15/17.
//

#include "SSLTest.h"

using namespace smooth::network;

SSLTest::SSLTest()
        : Task("SSLTest", 4096, 5, std::chrono::milliseconds(10)),
          txEmpty("txEmpty", 1, *this, *this),
          data_available("data_available", 20, *this, *this),
          connection_status("connection_status", 3, *this, *this),
          tx(),
          rx(),
          s(tx, rx, txEmpty, data_available, connection_status)
{
    auto ip = std::make_shared<smooth::network::IPv4>("172.217.18.142", 443);
    s.start(ip);
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
    if (!done && msg.is_connected())
    {
        done = true;
        HTTPPacket sp("GET / HTTP/1.1\r\n"
                              "Connection: close"
                              "\r\n\r\n");
        tx.put(sp);
    }
}