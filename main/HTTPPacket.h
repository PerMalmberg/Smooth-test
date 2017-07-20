//
// Created by permal on 7/15/17.
//

#pragma once

#include <string>
#include <map>
#include <algorithm>
#include <smooth/core/network/ISendablePacket.h>
#include <smooth/core/network/IReceivablePacket.h>

class HTTPPacket
        : public smooth::core::network::ISendablePacket, public smooth::core::network::IReceivablePacket
{
    public:
        HTTPPacket() = default;

        explicit HTTPPacket(const std::string& data);

        // Must return the total amount of bytes to send
        int get_send_length() override;

        // Must return a pointer to the data to be sent.
        const uint8_t* get_data() override;

        // Must return the number of bytes the packet wants to fill
        // its internal buffer, e.g. header, checksum etc. Returned
        // value will differ depending on how much data already has been provided.
        int get_wanted_amount() override;

        // Used by the underlying framework to notify the packet that {length} bytes
        // has been written to the buffer pointed to by get_write_pos().
        // During the call to this method the packet should do whatever it needs to
        // evaluate if it needs more data or if it is complete.
        void data_received(int length) override;

        // Must return the current write position of the internal buffer.
        // Must point to a buffer than can accept the number of bytes returned by
        // get_wanted_amount().
        uint8_t* get_write_pos() override;

        // Must return true when the packet has received all data it needs
        // to fully assemble.
        bool is_complete() override;

        // Must return true whenever the packet is unable to correctly assemble
        // based on received data.
        bool is_error() override;

        const std::string& to_string();


    private:
        void split_header(const std::string& s);

        int curr_len = 0;
        std::string data{};
        uint8_t buff[2]{0, 0};
        bool completed = false;
        std::string status{};
        std::string body{};
        std::map<std::string, std::string> header{};
        bool reading_status_line = true;
        bool reading_header = true;
        int remaining_bytes = 1;
};