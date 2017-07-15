//
// Created by permal on 7/15/17.
//

#pragma once

#include <smooth/network/ISendablePacket.h>
#include <smooth/network/IReceivablePacket.h>

class StreamingStringPacket
        : public smooth::network::ISendablePacket, public smooth::network::IReceivablePacket
{
    public:
        StreamingStringPacket() = default;

        explicit StreamingStringPacket(const std::string& data)
                : data(data)
        {
        }

        // Must return the total amount of bytes to send
        int get_send_length() override
        {
            return static_cast<int>(data.length());
        }

        // Must return a pointer to the data to be sent.
        const uint8_t* get_data() override
        {
            return reinterpret_cast<const uint8_t*>(data.c_str());
        }

        // Must return the number of bytes the packet wants to fill
        // its internal buffer, e.g. header, checksum etc. Returned
        // value will differ depending on how much data already has been provided.
        int get_wanted_amount() override
        {
            // Read byte by byte.
            return 1;
        }

        // Used by the underlying framework to notify the packet that {length} bytes
        // has been written to the buffer pointed to by get_write_pos().
        // During the call to this method the packet should do whatever it needs to
        // evaluate if it needs more data or if it is complete.
        void data_received(int length) override
        {
            curr_len += length;
            data += buff[0];
            if( buff[0] == '\n' || buff[0] == '\r')
            {
                completed = true;
            }
        }

        // Must return the current write position of the internal buffer.
        // Must point to a buffer than can accept the number of bytes returned by
        // get_wanted_amount().
        uint8_t* get_write_pos() override
        {
            return buff;
        }

        // Must return true when the packet has received all data it needs
        // to fully assemble.
        bool is_complete() override
        {
            // Simply read in chunks of 50 for demo purposes.
            return completed;
        }

        // Must return true whenever the packet is unable to correctly assemble
        // based on received data.
        bool is_error() override
        {
            return false;
        }

        const std::string& to_string()
        {
            return data;
        }

    private:
        int curr_len = 0;
        std::string data{};
        uint8_t buff[2]{0, 0};
        bool completed = false;
};