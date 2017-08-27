#include <driver/gpio.h>
#include <smooth/core/network/Wifi.h>
#include <smooth/core/ipc/TaskEventQueue.h>
#include <smooth/application/network/mqtt/MqttClient.h>
#include <smooth/core/Application.h>
#include <smooth/application/display/ST7735.h>
//#include <smooth/core/io/spi/Master.h>
#include <smooth/core/io/i2c/Master.h>
#include <smooth/application/sensor/BME280.h>
#include <smooth/application/io/MCP23017.h>
#include <smooth/core/io/Input.h>
#include <smooth/core/io/Output.h>
#include "esp_system.h"


#undef write
#undef read

#include <sstream>

#include "wifi-creds.h"

using namespace smooth::core;
using namespace smooth::core::ipc;
using namespace smooth::core::network;
using namespace std::chrono;
using namespace smooth::application::network::mqtt;
using namespace smooth::application::display;
using namespace smooth::application::sensor;
using namespace smooth::application::io;

static const std::string mqtt_broker = "192.168.10.247";

class MyApp
        : public Application,
          public IEventListener<MQTTData>
{
    public:
        MyApp() : Application(tskIDLE_PRIORITY + 1, std::chrono::milliseconds(1000)),
                  mqtt_data("mqtt_data", 10, *this, *this),
                  mqtt("TestMQTT", std::chrono::seconds(30), 4096, tskIDLE_PRIORITY + 1, mqtt_data),
                  i2c(I2C_NUM_0, GPIO_NUM_25, true, GPIO_NUM_26, true, 1000000),
                  i2c_2(I2C_NUM_1, GPIO_NUM_14, true, GPIO_NUM_27, true, 1000000),
                  led(GPIO_NUM_5, true),
                  gpb0(GPIO_NUM_15, true),
                  gpb1(GPIO_NUM_5, true),
                  gpb2(GPIO_NUM_18, true),
                  gpb3(GPIO_NUM_23, true),
                  gpb4(GPIO_NUM_19, true),
                  gpb5(GPIO_NUM_22, true),
                  gpb6(GPIO_NUM_21, true),
                  gpb7(GPIO_NUM_3, true)
        {
        }

        void init() override
        {
            Application::init();
/*
            mqtt.start();
            auto address = std::make_shared<smooth::core::network::IPv4>(mqtt_broker, 1883);
            mqtt.connect_to(address, true);
            mqtt.subscribe("HAP/humidity", QoS::AT_MOST_ONCE);
*/
            bme280 = i2c.create_device<BME280>(0x76);

            if (bme280)
            {
                ESP_LOGV("main", "BME280 id: %x", bme280->read_id());
                ESP_LOGV("main", "BME280 configure_sensor: %d", bme280->configure_sensor(
                        BME280::SensorMode::Normal,
                        BME280::OverSampling::Oversamplingx16,
                        BME280::OverSampling::Oversamplingx16,
                        BME280::OverSampling::Oversamplingx16,
                        BME280::StandbyTimeMS::ST_0_5,
                        BME280::FilterCoeff::FC_16));
            }

            mcp23017 = i2c_2.create_device<MCP23017>(0x20);
            if (mcp23017)
            {
                bool present = mcp23017->is_present();
                bool known = mcp23017->put_device_into_known_state(false);
                ESP_LOGV("MCP23017 detected", "%d", present);
                ESP_LOGV("MCP23017 state set", "%d", known);
                if (present && known)
                {
                    // Set all port B I/O as intput
                    mcp23017->configure_ports(0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0x00);
                }
                else
                {
                    mcp23017.reset();
                }
            }
        }

        void event(const MQTTData& event) override
        {
            std::stringstream ss;
            for (auto& b : event.second)
            {
                ss << static_cast<char>(b);
            }

            ESP_LOGV("Received", "%s: %s", event.first.c_str(), ss.str().c_str());
        }

        void tick() override
        {
            ESP_LOGV("Main", "Free heap: %u", esp_get_free_heap_size());

            if (mcp23017)
            {
                ++ix;
                if (ix == 8)
                {
                    ix = 0;
                }
                ESP_LOGV("ix", "%d", ix);

                const std::array<io::Output*, 8> outs{&gpb0, &gpb1, &gpb2, &gpb3, &gpb4, &gpb5, &gpb6, &gpb7};

                for (int i = 0; i < outs.size(); ++i)
                {
                    outs[i]->clr();
                }
                outs[ix]->set();

                uint8_t value;
                if(mcp23017->read_input(MCP23017::Port::B, value))
                {

                    ESP_LOGV("Read", "%d %d %d %d %d %d %d %d",
                             (value & 0x80) > 0,
                             (value & 0x40) > 0,
                             (value & 0x20) > 0,
                             (value & 0x10) > 0,
                             (value & 0x08) > 0,
                             (value & 0x04) > 0,
                             (value & 0x02) > 0,
                             (value & 0x01) > 0);
                }
            }
/*
            if (bme280)
            {
                float hum, press, temp;
                bool res = bme280->read_measurements(hum, press, temp);
                if (res)
                {
                    ss.str("");
                    ss << hum;
                    mqtt.publish("HAP/humidity", ss.str(), EXACTLY_ONCE, false);
                    ss.str("");
                    ss << press / 100; // to hPa
                    mqtt.publish("HAP/press", ss.str(), AT_MOST_ONCE, false);
                    ss.str("");
                    ss << temp;
                    mqtt.publish("HAP/temp", ss.str(), AT_LEAST_ONCE, false);
                    ss.str("");
                    ss << esp_get_free_heap_size();
                    mqtt.publish("HAP/heap", ss.str(), AT_LEAST_ONCE, false);
                }
            }

            if (!mqtt.is_connected())
            {
                led.set();
                delay(milliseconds(500));
                led.clr();
            }
            */
        }

    private:
        smooth::core::ipc::TaskEventQueue<MQTTData> mqtt_data;
        smooth::application::network::mqtt::MqttClient mqtt;
        smooth::core::io::i2c::Master i2c;
        smooth::core::io::i2c::Master i2c_2;
        std::unique_ptr<BME280> bme280{};
        std::unique_ptr<MCP23017> mcp23017;
        std::stringstream ss{};
        smooth::core::io::Output led;
        uint8_t ix = 0;
        io::Output gpb0;
        io::Output gpb1;
        io::Output gpb2;
        io::Output gpb3;
        io::Output gpb4;
        io::Output gpb5;
        io::Output gpb6;
        io::Output gpb7;
};

extern "C" void app_main()
{
/*    io::spi::Master spi(HSPI_HOST,
                      io::spi::SPI_DMA_Channel::DMA_1,
                      GPIO_NUM_25,
                      static_cast<gpio_num_t>(-1),
                      GPIO_NUM_19);

    ST7735 display(spi, GPIO_NUM_22, GPIO_NUM_21, GPIO_NUM_18, GPIO_NUM_5);
    display.initialize();
    display.set_back_light(true);
    display.software_reset();
*/
    // Create the application, it will run on the main task
    // so set an appropriate stack size in the config.
    MyApp app;

    Wifi& wifi = app.get_wifi();
    wifi.set_host_name("HAP-ESP32");
    wifi.set_ap_credentials(WIFI_SSID, WIFI_PASSWORD);
    wifi.set_auto_connect(true);
    app.set_system_log_level(ESP_LOG_ERROR);

    // Start the application. Note that this function never returns.
    app.start();
}