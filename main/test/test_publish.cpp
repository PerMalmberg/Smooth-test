#ifdef TEST_PUBLISH

#include "test_publish.h"
#include <smooth/core/task_priorities.h>
#include <smooth/core/json/Value.h>
#include <fstream>
#include <cassert>
#include <smooth/core/logging/log.h>
#include <chrono>
#include <smooth/core/ipc/SubscribingTaskEventQueue.h>
#include <smooth/core/ipc/Publisher.h>
#include <smooth/core/Application.h>

using namespace smooth;

using namespace smooth::core;
using namespace smooth::core::json;
using namespace smooth::core::ipc;
using namespace smooth::core::logging;
using namespace std::chrono;

PublisherTask::PublisherTask()
    : smooth::core::Task("Publisher", 8192, smooth::core::APPLICATION_BASE_PRIO, milliseconds(1))
{
}

void PublisherTask::init()
{
}

void PublisherTask::tick()
{
    smooth::core::ipc::Publisher<ItemToPublish>::publish(ItemToPublish(++curr));
}


TestApp::TestApp()
        : Application(APPLICATION_BASE_PRIO, seconds(1)),
          sub("sub", 10, *this, *this)
{
}

void TestApp::init()
{
    POSIXApplication::init();
    p.start();
}

void TestApp::tick()
{

}

void TestApp::event(const ItemToPublish& item)
{
    Log::info("event", Format("Got value: {1}", Int32(item.val)));
}


#endif // TEST_PUBLISH
