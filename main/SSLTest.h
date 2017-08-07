//
// Created by permal on 7/15/17.
//

#pragma once

#include <chrono>
#include <smooth/core/ipc/IEventListener.h>
#include <smooth/core/ipc/TaskEventQueue.h>
#include <smooth/core/network/DataAvailableEvent.h>
#include <smooth/core/network/TransmitBufferEmptyEvent.h>
#include <smooth/core/network/ConnectionStatusEvent.h>
#include <smooth/core/network/PacketSendBuffer.h>
#include <smooth/core/network/SSLSocket.h>
#include <smooth/core/network/IPv4.h>
#include <smooth/core/Task.h>
#include "HTTPPacket.h"
#include <smooth/core/timer/Timer.h>

class SSLTest
        : public smooth::core::Task,
          public smooth::core::ipc::IEventListener<smooth::core::network::DataAvailableEvent<HTTPPacket>>,
          public smooth::core::ipc::IEventListener<smooth::core::network::TransmitBufferEmptyEvent>,
          public smooth::core::ipc::IEventListener<smooth::core::network::ConnectionStatusEvent>,
          public smooth::core::ipc::IEventListener<smooth::core::timer::TimerExpiredEvent>
{
    public:
        SSLTest();

        void event(const smooth::core::network::DataAvailableEvent<HTTPPacket>& msg) override;
        void event(const smooth::core::network::TransmitBufferEmptyEvent& msg) override;
        void event(const smooth::core::network::ConnectionStatusEvent& msg) override;
        void event(const smooth::core::timer::TimerExpiredEvent& msg) override;

    protected:
        void init() override;

    private:
        smooth::core::ipc::TaskEventQueue<smooth::core::network::TransmitBufferEmptyEvent> txEmpty;
        smooth::core::ipc::TaskEventQueue<smooth::core::network::DataAvailableEvent<HTTPPacket>> data_available;
        smooth::core::ipc::TaskEventQueue<smooth::core::network::ConnectionStatusEvent> connection_status;
        smooth::core::network::PacketSendBuffer<HTTPPacket, 1> tx;
        smooth::core::network::PacketReceiveBuffer<HTTPPacket, 2> rx;
        std::shared_ptr<smooth::core::network::ISocket> s;
        smooth::core::ipc::TaskEventQueue<smooth::core::timer::TimerExpiredEvent> timer_expired;
        smooth::core::timer::Timer timer;
};
