#ifdef TEST_QUEUE

#include "test_queue.h"
#include <smooth/core/logging/log.h>

using namespace smooth;
using namespace smooth::core;
using namespace smooth::core::ipc;
using namespace smooth::core::logging;
using namespace std::chrono;

TestApp::TestApp()
        : Application(5, std::chrono::milliseconds(1000)),
          queue("string queue", 10, *this, *this),
          sender(queue)
{
}

void TestApp::init()
{
    Application::init();
    sender.start();
}

void TestApp::event(const timer::ElapsedTime& event)
{
    auto val = event.get_running_time();
    total += val;
    count++;

    if (count % 100 == 0)
    {
        Log::debug("Stats", Format("Avg: {1}us per message, Count: {2}, Last msg: {3}us",
                                   Double(total.count() / count, 2),
                                   Int64(count),
                                   Int64(val.count())));
    }
}

SenderTask::SenderTask(TaskEventQueue<timer::ElapsedTime>& out) :
        core::Task("SenderTask", 4096, 10, milliseconds(1)),
        out(out)
{
}

void SenderTask::tick()
{
    timer::ElapsedTime e;
    e.start();
    out.push(e);
}

#endif // TEST_QUEUE