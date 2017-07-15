//
// Created by permal on 7/15/17.
//

#include "HTTPPacket.h"
#include <esp_log.h>

HTTPPacket::HTTPPacket(const std::string& data)
        : data(data)
{
}

int HTTPPacket::get_send_length()
{
    return static_cast<int>(data.length());
}

const uint8_t* HTTPPacket::get_data()
{
    return reinterpret_cast<const uint8_t*>(data.c_str());
}

int HTTPPacket::get_wanted_amount()
{
    return 1;
}

void HTTPPacket::data_received(int length)
{
    curr_len += length;
    data += buff[0];

    if (reading_status_line)
    {
        if (data.length() >= 2
            && data[data.length() - 2] == '\r'
            && data[data.length() - 1] == '\n')
        {
            // Status line
            status = data;
            data.clear();
            reading_status_line = false;
        }
    }
    else if (reading_header)
    {
        // Look for line containing only "<cr><lf>"
        if (data.length() == 2 && data.compare("\r\n") == 0)
        {
            // End of header found
            reading_header = false;
            data.clear();
            auto content_length = header.find("Content-Length");
            if (content_length != header.end())
            {
                remaining_bytes = atoi(content_length->second.c_str());
            }
        }
        else if (data.length() >= 2
                 && data[data.length() - 2] == '\r'
                 && data[data.length() - 1] == '\n')
        {
            // Header line
            split_header(data);
            data.clear();
        }
    }
    else
    {
        if( --remaining_bytes <= 0)
        {
            completed = true;
            body = data;
            data.clear();
        }

    }
}

// Must return the current write position of the internal buffer.
// Must point to a buffer than can accept the number of bytes returned by
// get_wanted_amount().
uint8_t* HTTPPacket::get_write_pos()
{
    return buff;
}

// Must return true when the packet has received all data it needs
// to fully assemble.
bool HTTPPacket::is_complete()
{
    // Simply read in chunks of 50 for demo purposes.
    return completed;
}

// Must return true whenever the packet is unable to correctly assemble
// based on received data.
bool HTTPPacket::is_error()
{
    return false;
}

const std::string& HTTPPacket::to_string()
{
    return body;
}

void HTTPPacket::split_header(const std::string& s)
{
    auto colon_ix = s.find(':');
    auto space_ix = s.find(' ');

    if (colon_ix != std::string::npos && space_ix != std::string::npos)
    {
        auto key = s.substr(0, colon_ix);
        // Skip ending \n\r in value
        auto value = s.substr(space_ix + 1);
        header.insert(std::make_pair(key, value));
    }
}