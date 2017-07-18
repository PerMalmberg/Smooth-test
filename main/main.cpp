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
#include <smooth/timer/Timer.h>
#include "SSLTest.h"

#include "wifi-creds.h"

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

class LedControl
        : public smooth::Task,
          public smooth::ipc::IEventListener<network::DataAvailableEvent<TestPacket>>,
          public smooth::ipc::IEventListener<network::TransmitBufferEmptyEvent>,
          public smooth::ipc::IEventListener<network::ConnectionStatusEvent>,
          public smooth::ipc::IEventListener<timer::TimerExpiredEvent>
{
    public:

        explicit LedControl() :
                Task("LedControl", 8192, 5, milliseconds(10)),
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

        void init() override
        {
            gpio_pad_select_gpio(GPIO_NUM_5);
            gpio_set_direction(GPIO_NUM_5, GPIO_MODE_OUTPUT);
            gpio_set_level(GPIO_NUM_5, 0);

            gpio_pad_select_gpio(GPIO_NUM_26);
            gpio_set_direction(GPIO_NUM_26, GPIO_MODE_OUTPUT);
            gpio_set_level(GPIO_NUM_26, 0);

            network_timer.start();
            steady_blink.start();
            auto ip = std::make_shared<IPv4>("192.168.10.44", 5566);
            s.start(ip);
        }

        void message(const DataAvailableEvent<TestPacket>& msg) override
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

        void message(const TransmitBufferEmptyEvent& msg) override
        {
            if (stress)
            {
                tx.put(TestPacket());
            }
        }

        void message(const ConnectionStatusEvent& msg) override
        {
            ESP_LOGV("Conn", "%s", msg.is_connected() ? "Connected" : "Not connected");
        }

        void message(const timer::TimerExpiredEvent& msg) override
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

    private:
        TaskEventQueue<TransmitBufferEmptyEvent> txEmpty;
        TaskEventQueue<DataAvailableEvent<TestPacket>> data_available;
        TaskEventQueue<ConnectionStatusEvent> connection_status;
        TaskEventQueue<timer::TimerExpiredEvent> timer_expired;
        smooth::network::PacketSendBuffer<TestPacket, 1> tx;
        smooth::network::PacketReceiveBuffer<TestPacket, 5> rx;
        smooth::network::Socket<TestPacket> s;
        timer::Timer network_timer;
        timer::Timer steady_blink;
        bool stress = false;
        bool led_on = false;
};


extern "C" void app_main()
{
    // Start socket dispatcher first of all so that it is
    // ready to receive network status events.
    SocketDispatcher::instance();

    Application app("Main app", 4096, 6);
    //app.set_system_log_level(ESP_LOG_ERROR);
    app.start();

    Wifi wifi;
    wifi.connect_to_ap("HAP-ESP32", WIFI_SSID, WIFI_PASSWORD, true);

    vTaskDelay(pdMS_TO_TICKS(5000));

    LedControl led;
    led.start();

    SSLTest ssl_test;
    ssl_test.start();

    ESP_LOGV("Main", "Free heap: %u", esp_get_free_heap_size());

    smooth::Task::never_return();
}