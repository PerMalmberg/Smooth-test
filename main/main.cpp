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
#include <smooth/application/network/mqtt/MqttClient.h>
#include "esp_system.h"
#include <sstream>
#include <thread>


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
        : public Application,
          public core::ipc::IEventListener<timer::ElapsedTime>
{
    public:

        MyApp() : Application(tskIDLE_PRIORITY + 1, std::chrono::milliseconds(1000)),
                  queue("string queue", 10, *this, *this),
                  sender(queue)
        {
        }

        class SenderTask
                : public core::Task
        {
            public:
                SenderTask(TaskEventQueue<timer::ElapsedTime>& out) :
                        core::Task("Foo", 4096, 10, milliseconds(100)),
                        out(out)
                {
                }

                void tick() override
                {
                    timer::ElapsedTime e;
                    e.start();
                    out.push(e);
                }

            private:
                TaskEventQueue<timer::ElapsedTime>& out;


        };

        void init() override
        {
            Application::init();
            sender.start();
        }

        void tick() override
        {
            print_task_info();
        }

        void event(const timer::ElapsedTime& event)
        {
            ESP_LOGV("int", "%llu", event.get_running_time().count());
        }

    private:
        core::ipc::TaskEventQueue<timer::ElapsedTime> queue;
        SenderTask sender;
};

extern "C" void app_main()
{
    // Create the application, it will run on the main task
    // so set an appropriate stack size in the config.
    MyApp app;

    // Start the application. Note that this function never returns.
    app.start();
}
