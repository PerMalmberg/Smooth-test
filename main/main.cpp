#include <driver/gpio.h>
#include <smooth/network/Wifi.h>
#include <smooth/Task.h>
#include <smooth/network/IPv4.h>
#include <smooth/network/IPv6.h>
#include <smooth/network/Socket.h>
#include <smooth/Application.h>
#include <smooth/ipc/Publisher.h>
#include <esp_system.h>
#include <smooth/network/ISendablePacket.h>
#include <smooth/network/IReceivablePacket.h>
#include <smooth/network/SSLSocket.h>

#include "wifi-creds.h"

const gpio_num_t BLINK_GPIO = GPIO_NUM_5;

using namespace smooth;
using namespace smooth::ipc;
using namespace smooth::network;
using namespace std::chrono;

class TestPacket
        : public ISendablePacket, public IReceivablePacket
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

class BlinkReceive
        : public smooth::Task,
          public smooth::ipc::IEventListener<network::DataAvailable<TestPacket>>,
          public smooth::ipc::IEventListener<network::TransmitBufferEmpty>,
          public smooth::ipc::IEventListener<network::ConnectionStatus>
{
    public:
        explicit BlinkReceive() :
                Task("BlinkReceive", 14096, 5, milliseconds(10)),
                txEmpty("txEmpty", 1, *this, *this),
                data_available("data_available", 5, *this, *this),
                connection_status("connection_status", 3, *this, *this),
                tx(),
                rx(),
                s(tx, rx, txEmpty, data_available, connection_status)
        {
            gpio_pad_select_gpio(BLINK_GPIO);
            gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
            gpio_set_level(BLINK_GPIO, 0);

            auto ip = std::make_shared<IPv4>("192.168.10.44", 5566);
            s.start(ip);
        }

        void message(const DataAvailable<TestPacket>& msg) override
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
                    gpio_set_level(BLINK_GPIO, 1);
                }
                else if (data.get_data()[0] == '0')
                {
                    gpio_set_level(BLINK_GPIO, 0);
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

        void message(const TransmitBufferEmpty& msg) override
        {
            if (stress)
            {
                tx.put(TestPacket());
            }
        }

        void message(const ConnectionStatus& msg) override
        {
            if (!msg.is_connected())
            {
                s.restart();
            }
        }

    private:
        TaskEventQueue<TransmitBufferEmpty> txEmpty;
        TaskEventQueue<DataAvailable<TestPacket>> data_available;
        TaskEventQueue<ConnectionStatus> connection_status;
        smooth::network::PacketSendBuffer<TestPacket, 1> tx;
        smooth::network::PacketReceiveBuffer<TestPacket, 5> rx;
        smooth::network::Socket<TestPacket> s;
        bool stress = false;
};

class StreamingStringPacket
        : public ISendablePacket, public IReceivablePacket
{
    public:
        StreamingStringPacket() = default;

        explicit StreamingStringPacket(const std::string& data)
                : data(data)
        {
        }

        // Must return the total amount of bytes to send
        int get_send_length() override
        {
            return static_cast<int>(data.length());
        }

        // Must return a pointer to the data to be sent.
        const uint8_t* get_data() override
        {
            return reinterpret_cast<const uint8_t*>(data.c_str());
        }

        // Must return the number of bytes the packet wants to fill
        // its internal buffer, e.g. header, checksum etc. Returned
        // value will differ depending on how much data already has been provided.
        int get_wanted_amount() override
        {
            // Read byte by byte.
            return 1;
        }

        // Used by the underlying framework to notify the packet that {length} bytes
        // has been written to the buffer pointed to by get_write_pos().
        // During the call to this method the packet should do whatever it needs to
        // evaluate if it needs more data or if it is complete.
        void data_received(int length) override
        {
            curr_len += length;
            data += buff[0];
            if( buff[0] == '\n' || buff[0] == '\r')
            {
                completed = true;
            }
        }

        // Must return the current write position of the internal buffer.
        // Must point to a buffer than can accept the number of bytes returned by
        // get_wanted_amount().
        uint8_t* get_write_pos() override
        {
            return buff;
        }

        // Must return true when the packet has received all data it needs
        // to fully assemble.
        bool is_complete() override
        {
            // Simply read in chunks of 50 for demo purposes.
            return completed;
        }

        // Must return true whenever the packet is unable to correctly assemble
        // based on received data.
        bool is_error() override
        {
            return false;
        }

        const std::string& to_string()
        {
            return data;
        }

    private:
        int curr_len = 0;
        std::string data{};
        uint8_t buff[2]{0, 0};
        bool completed = false;
};

class SSLTest
        : public smooth::Task,
          public smooth::ipc::IEventListener<network::DataAvailable<StreamingStringPacket>>,
          public smooth::ipc::IEventListener<network::TransmitBufferEmpty>,
          public smooth::ipc::IEventListener<network::ConnectionStatus>
{
    public:
        explicit SSLTest() :
                Task("SSLTest", 4096, 5, milliseconds(10)),
                txEmpty("txEmpty", 1, *this, *this),
                data_available("data_available", 40, *this, *this),
                connection_status("connection_status", 3, *this, *this),
                tx(),
                rx(),
                s(tx, rx, txEmpty, data_available, connection_status)
        {
            auto ip = std::make_shared<IPv4>("172.217.18.142", 443);
            s.start(ip);
        }

        void message(const DataAvailable<StreamingStringPacket>& msg) override
        {
            StreamingStringPacket data;
            if (msg.get(data))
            {
                ESP_LOGV( "SSP", "%s", data.to_string().c_str());
            }
        }

        void message(const TransmitBufferEmpty& msg) override
        {
            ESP_LOGV("SSP", "Packet sent");
        }

        void message(const ConnectionStatus& msg) override
        {
            if (!done && msg.is_connected())
            {
                done = true;
                StreamingStringPacket sp("GET / HTTP/1.1\r\n\r\n");
                tx.put(sp);
            }
        }

    private:
        TaskEventQueue<TransmitBufferEmpty> txEmpty;
        TaskEventQueue<DataAvailable<StreamingStringPacket>> data_available;
        TaskEventQueue<ConnectionStatus> connection_status;
        smooth::network::PacketSendBuffer<StreamingStringPacket, 1> tx;
        smooth::network::PacketReceiveBuffer<StreamingStringPacket, 50> rx;
        smooth::network::SSLSocket<StreamingStringPacket> s;
        bool done = false;
};


extern "C" void app_main()
{

    Application app("Main app", 4096, 6);
    app.set_system_log_level(ESP_LOG_ERROR);
    app.start();

    Wifi wifi;
    wifi.connect_to_ap("HAP-ESP32", WIFI_SSID, WIFI_PASSWORD, true);

 //   BlinkReceive r;
 //   r.start();

    SSLTest ssl_test;
    ssl_test.start();

    SocketDispatcher::instance();

    ESP_LOGV("Main", "Free heap: %u", esp_get_free_heap_size());

    smooth::Task::never_return();
}