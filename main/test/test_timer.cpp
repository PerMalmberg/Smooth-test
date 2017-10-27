#ifdef TEST_TIMER

#include "test_timer.h"

using namespace smooth;
using namespace smooth::core;
using namespace smooth::core::timer;
using namespace smooth::core::ipc;
using namespace smooth::core::logging;
using namespace std::chrono;

TestApp::TestApp()
        : Application(10, seconds(10)),
          queue("queue", 10, *this, *this),
          timers()
{
}

void TestApp::init()
{
    Application::init();

    create_timer(milliseconds(1000));
    create_timer(milliseconds(3000));
    create_timer(milliseconds(5000));
    create_timer(milliseconds(10000));

    for (auto& t:timers)
    {
        t.timer->start();
    }

    timers.front().timer->stop();
    timers.front().timer->start();
}

void TestApp::event(const smooth::core::timer::TimerExpiredEvent& event)
{
    auto& info = timers[event.get_id()];
    milliseconds duration = duration_cast<milliseconds>(steady_clock::now() - info.last);
    info.last = steady_clock::now();
    info.count++;
    info.total += duration;

    Log::verbose("Interval", Format("{1} ({2}ms): {3}ms, avg: {4}",
                                    Int32(event.get_id()),
                                    Int64(info.interval.count()),
                                    Int64(duration.count()),
                                    Double(info.total.count() * 1.0 / info.count)));
}

void TestApp::create_timer(std::chrono::milliseconds interval)
{
    TimerInfo t;
    t.timer = Timer::create("Timer", static_cast<int32_t>(timers.size()), queue, true, interval);
    t.interval = interval;
    timers.push_back(t);
}


#endif // TEST_QUEUE