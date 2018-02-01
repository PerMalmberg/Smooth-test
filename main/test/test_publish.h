#pragma once

#ifdef TEST_PUBLISH


#include <smooth/core/Application.h>

class ItemToPublish
{
    public:
        ItemToPublish() = default;

        explicit ItemToPublish(int v)
                : val(v)
        {
        }

        int val = 0;
};

class PublisherTask
        : public smooth::core::Task
{
    public:
        PublisherTask();
        void init() override;
        void tick() override;
    private:
        int curr = 0;
};


class TestApp
        : public smooth::core::Application,
          smooth::core::ipc::IEventListener<ItemToPublish>
{
    public:
        TestApp();
        void init() override;
        void tick() override;
        void event(const ItemToPublish&) override;
    private:
        smooth::core::ipc::SubscribingTaskEventQueue<ItemToPublish> sub;
        PublisherTask p{};
};


#endif // TEST_PUBLISH