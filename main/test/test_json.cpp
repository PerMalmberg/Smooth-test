#ifdef TEST_JSON

#include "test_json.h"
#include <smooth/core/task_priorities.h>
#include <smooth/core/json/Json.h>
#include <fstream>
#include <assert.h>

using namespace smooth;

using namespace smooth::core;
using namespace smooth::core::json;
using namespace smooth::core::timer;
using namespace smooth::core::ipc;
using namespace smooth::core::logging;
using namespace std::chrono;

TestApp::TestApp()
        : Application(APPLICATION_BASE_PRIO, seconds(1))
{
}

void TestApp::init()
{
    POSIXApplication::init();
}

void TestApp::tick()
{
    auto data = read_file("../../main/test/test_example.json");
    if (data)
    {
        auto* json = cJSON_Parse(data.get());
        Value root{json};
        assert(root["key_with_string"] == "value");
        root["key_with_string"] = "new_value";
        assert(root["key_with_string"] == "new_value");

        Log::info("Data", Format("{1}", Str(cJSON_Print(json))));

        cJSON_Delete(json);
    }

}

std::unique_ptr<char[]> TestApp::read_file(const std::string& path)
{
    std::unique_ptr<char[]> data{};
    std::fstream f(path, std::ios::in);
    if (f.good())
    {
        auto size = f.seekg(0, f.end).tellg();
        f.seekg(0, f.beg);
        data = std::make_unique<char[]>(static_cast<size_t>(size) + sizeof(char));
        f.read(data.get(), size);
        data[size] = 0;
    }

    return data;
}


#endif // TEST_JSON