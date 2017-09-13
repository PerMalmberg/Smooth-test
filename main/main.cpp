#include <driver/gpio.h>
#include <smooth/core/network/Wifi.h>
#include <smooth/core/ipc/TaskEventQueue.h>
#include <smooth/core/ipc/ISRTaskEventQueue.h>
#include <smooth/application/network/mqtt/MqttClient.h>
#include <smooth/core/Application.h>
#include <smooth/application/display/ST7735.h>
//#include <smooth/core/io/spi/Master.h>
#include <smooth/core/io/i2c/Master.h>
#include <smooth/application/sensor/BME280.h>
#include <smooth/application/io/MCP23017.h>
#include <smooth/application/io/ADS1115.h>
#include <smooth/core/io/Input.h>
#include <smooth/core/io/InterruptInput.h>
#include <smooth/core/util/ByteSet.h>
#include <smooth/application/rgb_led/RGBLed.h>
#include "esp_system.h"


#undef write
#undef read

#include <sstream>

#include "wifi-creds.h"

using namespace smooth;
using namespace smooth::core;
using namespace smooth::core::util;
using namespace smooth::core::ipc;
using namespace smooth::core::network;
using namespace smooth::application::network::mqtt;
using namespace smooth::application::display;
using namespace smooth::application::sensor;
using namespace smooth::application::io;
using namespace std::chrono;

static const std::string mqtt_broker = "192.168.10.44";

class MyApp
        : public Application
{
    public:

        MyApp() : Application(tskIDLE_PRIORITY + 1, std::chrono::milliseconds(1000)),
                  i2c(I2C_NUM_0, GPIO_NUM_19, true, GPIO_NUM_22, true, 100000),
                  adc()
        {
        }

        void init() override
        {
            Application::init();

            adc = i2c.create_device<smooth::application::io::ADS1115>(0x48);
            bool res = adc->configure(
                    ADS1115::Multiplexer::Single_AIN0,
                    ADS1115::Range::FSR_6_144,
                    ADS1115::OperationalMode::Continuous,
                    ADS1115::DataRate::SPS_16,
                    ADS1115::ComparatorMode::Traditional,
                    ADS1115::Alert_Ready_Polarity::ActiveLow,
                    ADS1115::LatchingComparator::NonLatching,
                    ADS1115::AssertStrategy::AssertAfterFourConversion);

            ESP_LOGV("ACD", "Configured: %d", res);


        }


        void tick() override
        {

            if (adc)
            {
                uint16_t ain1, ain2, ain3, ain4;
                bool res = adc->read_conversion(ADS1115::Multiplexer::Single_AIN0, ain1)
                && adc->read_conversion(ADS1115::Multiplexer::Single_AIN1, ain2)
                && adc->read_conversion(ADS1115::Multiplexer::Single_AIN2, ain3)
                && adc->read_conversion(ADS1115::Multiplexer::Single_AIN3, ain4);

                ESP_LOGV("Data", "%d: %x %x %x %x", res, ain1, ain2, ain3, ain4);
            }

            Task::delay(milliseconds(1000));

        }


    private:
        smooth::core::io::i2c::Master i2c;
        std::unique_ptr<smooth::application::io::ADS1115> adc;

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

    /*Wifi& wifi = app.get_wifi();
    wifi.set_host_name("HAP-ESP32");
    wifi.set_ap_credentials(WIFI_SSID, WIFI_PASSWORD);
    wifi.set_auto_connect(true);
    app.set_system_log_level(ESP_LOG_ERROR);
*/
    // Start the application. Note that this function never returns.
    app.start();
}