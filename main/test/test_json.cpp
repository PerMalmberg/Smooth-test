#ifdef TEST_JSON

#include "test_json.h"
#include <smooth/core/task_priorities.h>
#include <smooth/core/json/Value.h>
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
        assert(root["key_with_int"] == 1);
        root["key_with_int"] = 5;
        assert(root["key_with_int"] == 5);
        int i = static_cast<int>(root["key_with_int"]);
        assert(i == 5);

        assert(root["key_with_object"]["key_in_object_with_string"] == "the string");

        auto copy = root["key_with_object"]["key_in_object_with_string"];
        assert(copy == "the string");

        root["key_with_object"]["key_in_object_with_string"] = "another string";
        assert(root["key_with_object"]["key_in_object_with_string"] == "another string");

        std::string str = static_cast<std::string>(root["key_with_object"]["key_in_object_with_string"]);
        assert(str == "another string");

        assert(root["key_with_object"]["key_in_object_with_double"] == 1.2345);
        root["key_with_object"]["key_in_object_with_double"] = 6.789;
        double d = static_cast<double>(root["key_with_object"]["key_in_object_with_double"]);
        assert(d == 6.789);

        assert(root["io"]["reference_values"]["digital"]["i0"]["ref"].get_bool(false));
        assert(!root["io"]["reference_values"]["digital"]["i1"]["ref"].get_bool(true));

        assert(root["array"].get_array_size() == 6);
        assert(root["array"][2] == "a2");
        assert(root["array"][5]["value"].get_bool(false));
        assert(!root["array"][5]["value2"].get_bool(true));

        root["array"][0] = "asdf";
        assert(root["array"][0] == "asdf");

        root["array"][1] = 1;
        assert(root["array"][1] == 1);

        root["array"][2] = 3.14;
        assert(root["array"][2] == 3.14);

        // Accessing non existing index - returns the current parent item
        assert(root["array"][100].get_array_size() == 6);

        // Change object value in array
        root["array"][5]["value2"].set(true);
        assert(root["array"][5]["value2"].get_bool(false));

        Value v{"{ \"key_to_be_copied\": 12345 }"};
        root["key_with_empty_object"] = v;
        assert(root["key_with_empty_object"]["key_to_be_copied"] == 12345);

        // Non existing key -> new item of object type so not equal to any string or number.
        // Accessing a non existing key creates it.
        assert(root["non_existing_key"] != "");
        assert(root["non_existing_key"] != 0);
        assert(root["non_existing_key"] != 0.0);

        assert(root["key_with_true"].get_bool(false));
        assert(!root["key_with_false"].get_bool(true));


        // We can completely change the type of the held object
        root["non_existing_key"] = "asdf";
        assert(root["non_existing_key"] == "asdf");
        root["non_existing_key"] = 456;
        assert(root["non_existing_key"] == 456);
        root["non_existing_key"] = 8.9;
        assert(root["non_existing_key"] == 8.9);

        root["nested1"]["nested2"]["nested3"] = "I'm six feet under";
        assert(root["nested1"]["nested2"]["nested3"] == "I'm six feet under");

        auto print = cJSON_Print(json);
        Log::info("Data", Format("{1}", Str(print)));
        free(print);

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