//
// Created by permal on 7/15/17.
//

#pragma once

#include <chrono>
#include <smooth/ipc/IEventListener.h>
#include <smooth/ipc/TaskEventQueue.h>
#include <smooth/network/DataAvailable.h>
#include <smooth/network/TransmitBufferEmpty.h>
#include <smooth/network/ConnectionStatus.h>
#include <smooth/network/PacketSendBuffer.h>
#include <smooth/network/SSLSocket.h>
#include <smooth/network/IPv4.h>
#include <smooth/Task.h>
#include "HTTPPacket.h"

class SSLTest
        : public smooth::Task,
          public smooth::ipc::IEventListener<smooth::network::DataAvailable<HTTPPacket>>,
          public smooth::ipc::IEventListener<smooth::network::TransmitBufferEmpty>,
          public smooth::ipc::IEventListener<smooth::network::ConnectionStatus>
{
    public:
        SSLTest();

        void message(const smooth::network::DataAvailable<HTTPPacket>& msg) override;

        void message(const smooth::network::TransmitBufferEmpty& msg) override;

        void message(const smooth::network::ConnectionStatus& msg) override;

    private:
        smooth::ipc::TaskEventQueue<smooth::network::TransmitBufferEmpty> txEmpty;
        smooth::ipc::TaskEventQueue<smooth::network::DataAvailable<HTTPPacket>> data_available;
        smooth::ipc::TaskEventQueue<smooth::network::ConnectionStatus> connection_status;
        smooth::network::PacketSendBuffer<HTTPPacket, 1> tx;
        smooth::network::PacketReceiveBuffer<HTTPPacket, 2> rx;
        smooth::network::SSLSocket<HTTPPacket> s;
        bool done = false;
};
