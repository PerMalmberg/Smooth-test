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
#include "StreamingStringPacket.h"

class SSLTest
        : public smooth::Task,
          public smooth::ipc::IEventListener<smooth::network::DataAvailable<StreamingStringPacket>>,
          public smooth::ipc::IEventListener<smooth::network::TransmitBufferEmpty>,
          public smooth::ipc::IEventListener<smooth::network::ConnectionStatus>
{
    public:
        SSLTest();

        void message(const smooth::network::DataAvailable<StreamingStringPacket>& msg) override;

        void message(const smooth::network::TransmitBufferEmpty& msg) override;

        void message(const smooth::network::ConnectionStatus& msg) override;

    private:
        smooth::ipc::TaskEventQueue<smooth::network::TransmitBufferEmpty> txEmpty;
        smooth::ipc::TaskEventQueue<smooth::network::DataAvailable<StreamingStringPacket>> data_available;
        smooth::ipc::TaskEventQueue<smooth::network::ConnectionStatus> connection_status;
        smooth::network::PacketSendBuffer<StreamingStringPacket, 1> tx;
        smooth::network::PacketReceiveBuffer<StreamingStringPacket, 50> rx;
        smooth::network::SSLSocket<StreamingStringPacket> s;
        bool done = false;
};
