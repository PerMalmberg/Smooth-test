#include <driver/gpio.h>
#include <smooth/core/network/Wifi.h>
#include <smooth/core/ipc/TaskEventQueue.h>
#include <smooth/application/network/mqtt/MqttClient.h>
#include <smooth/core/Application.h>
#include <smooth/application/display/ST7735.h>
//#include <smooth/core/io/spi/Master.h>
#include <smooth/core/io/i2c/Master.h>
#include <smooth/application/sensor/BME280.h>
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

static const std::string mqtt_broker = "192.168.10.247";

class MyApp
        : public Application,
          public IEventListener<MQTTData>
{
    public:
        MyApp() : Application(tskIDLE_PRIORITY + 1, std::chrono::seconds(1)),
                  mqtt_data("mqtt_data", 10, *this, *this),
                  mqtt("TestMQTT", std::chrono::seconds(30), 4096, tskIDLE_PRIORITY + 1, mqtt_data),
                  i2c(I2C_NUM_0, GPIO_NUM_25, true, GPIO_NUM_26, true, 100000),
                  led(GPIO_NUM_5, true)
        {
        }

        void init() override
        {
            Application::init();

            mqtt.start();
            auto address = std::make_shared<smooth::core::network::IPv4>(mqtt_broker, 1883);
            mqtt.connect_to(address, true);
            mqtt.subscribe("HAP/humidity", QoS::AT_MOST_ONCE);

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
        }

        void event(const MQTTData& event) override
        {
            std::stringstream ss;
            for (auto& b : event.second)
            {
                ss << static_cast<char>(b);
            }

            led.clr();
            delay(milliseconds(200));
            led.set();

            ESP_LOGV("Received", "%s: %s", event.first.c_str(), ss.str().c_str());
        }

        void tick() override
        {
            ESP_LOGV("Main", "Free heap: %u", esp_get_free_heap_size());

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

            if (mqtt.is_connected())
            {
                for( int i = 0; i < 3; ++i)
                {
                    led.clr();
                    delay(milliseconds(50));
                    led.set();
                }
            }
            else
            {
                led.clr();
                delay(milliseconds(500));
                led.set();
                delay(milliseconds(500));
                led.clr();
            }
        }

    private:
        smooth::core::ipc::TaskEventQueue<MQTTData> mqtt_data;
        smooth::application::network::mqtt::MqttClient mqtt;
        smooth::core::io::i2c::Master i2c;
        std::unique_ptr<BME280> bme280{};
        std::stringstream ss{};
        smooth::core::io::Output led;
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