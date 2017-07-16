//
// Created by permal on 7/15/17.
//

#pragma once

#include <chrono>
#include <smooth/ipc/IEventListener.h>
#include <smooth/ipc/TaskEventQueue.h>
#include <smooth/network/DataAvailableEvent.h>
#include <smooth/network/TransmitBufferEmptyEvent.h>
#include <smooth/network/ConnectionStatusEvent.h>
#include <smooth/network/PacketSendBuffer.h>
#include <smooth/network/SSLSocket.h>
#include <smooth/network/IPv4.h>
#include <smooth/Task.h>
#include "HTTPPacket.h"

class SSLTest
        : public smooth::Task,
          public smooth::ipc::IEventListener<smooth::network::DataAvailableEvent<HTTPPacket>>,
          public smooth::ipc::IEventListener<smooth::network::TransmitBufferEmptyEvent>,
          public smooth::ipc::IEventListener<smooth::network::ConnectionStatusEvent>
{
    public:
        SSLTest();

        void message(const smooth::network::DataAvailableEvent<HTTPPacket>& msg) override;

        void message(const smooth::network::TransmitBufferEmptyEvent& msg) override;

        void message(const smooth::network::ConnectionStatusEvent& msg) override;

    private:
        smooth::ipc::TaskEventQueue<smooth::network::TransmitBufferEmptyEvent> txEmpty;
        smooth::ipc::TaskEventQueue<smooth::network::DataAvailableEvent<HTTPPacket>> data_available;
        smooth::ipc::TaskEventQueue<smooth::network::ConnectionStatusEvent> connection_status;
        smooth::network::PacketSendBuffer<HTTPPacket, 1> tx;
        smooth::network::PacketReceiveBuffer<HTTPPacket, 2> rx;
        smooth::network::SSLSocket<HTTPPacket> s;
        bool done = false;
};
