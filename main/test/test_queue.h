#pragma once

#ifdef TEST_QUEUE

#include <smooth/core/Application.h>
#include <smooth/core/ipc/IEventListener.h>
#include <smooth/core/timer/ElapsedTime.h>
#include <smooth/core/ipc/TaskEventQueue.h>

class SenderTask
        : public smooth::core::Task
{
    public:
        explicit SenderTask(smooth::core::ipc::TaskEventQueue<smooth::core::timer::ElapsedTime>& out);

        void tick() override;

    private:
        smooth::core::ipc::TaskEventQueue<smooth::core::timer::ElapsedTime>& out;
};

class TestApp
        : public smooth::core::Application,
          public smooth::core::ipc::IEventListener<smooth::core::timer::ElapsedTime>
{
    public:

        TestApp();

        void init() override;

        uint32_t count = 0;
        std::chrono::microseconds total = std::chrono::microseconds(0);

        void event(const smooth::core::timer::ElapsedTime& event);

    private:
        smooth::core::ipc::TaskEventQueue<smooth::core::timer::ElapsedTime> queue;
        SenderTask sender;
};

#endif // TEST_QUEUE