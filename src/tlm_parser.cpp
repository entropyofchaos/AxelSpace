#include "tlm_parser.h"

#include <algorithm>
#include <bit>
#include <span>

Parser::Parser() :
    buffer{},
    mCurrentPacketSize(0),
    mIsCompletePacket(0)
{
}

bool Parser::detectPkt(uint8_t byte)
{
    bool goodPacketFound = false;

    // Once we get a new packet, the held data isn't
    // considered valid anymore if it was before
    mIsCompletePacket = false;

    // Detect our header
    if (mCurrentPacketSize <= 3)
    {
        if (byte == MAGIC_HEADER[mCurrentPacketSize])
        {
            // We found a good header byte
            ++mCurrentPacketSize;
        }
        else
        {
            // If our header is corrupt, start again
            mCurrentPacketSize = 0;
        }
    } 
    else if (mCurrentPacketSize > 3)
    {
        // Retrieve rest of packet and validate checksum
        buffer[mCurrentPacketSize - MAGIC_HEADER_SIZE] = byte;
        ++mCurrentPacketSize;

        if (mCurrentPacketSize == PACKET_SIZE)
        {
            // Check if we have correct checksum
            if (calculateChecksum(std::span(buffer.data(), HEADER_PAYLOAD_SIZE)) 
                == buffer[buffer.size() - 1])
            {
                goodPacketFound = true;

                // Clear buffer
                mCurrentPacketSize = 0;

                // Allow for data extraction
                mIsCompletePacket = true;
            }
            else
            {
                // Check if there is a second MAGIC_HEADER inside our packet. For instance, we actually got
                // a bad set of bytes that somehow was our magic packet
                if (auto it = 
                    std::ranges::search(
                        buffer.begin(), buffer.end(),
                        MAGIC_HEADER.begin(), MAGIC_HEADER.end()
                    ).begin(); 
                    it != buffer.end())
                {
                    // Move the packet to the beginning of our buffer, ignoring the MAGIC_NUMBER, and try to parse
                    // the rest of the packet again as we receive more bytes
                    std::copy(it + MAGIC_HEADER_SIZE, buffer.end(), buffer.begin());

                    // Current packet size now is the amount of data we just moved over in our buffer
                    mCurrentPacketSize = static_cast<uint8_t>(std::distance(it, buffer.end()));
                }
                else
                {
                    // Try to see if part of the magic header is at the end of our packet in case the previous
                    // message got cut off prematurely
                    for (uint8_t i = 1; i < MAGIC_HEADER_SIZE; ++i) 
                    {
                        if (std::equal(buffer.end() - i, buffer.end(), MAGIC_HEADER.begin())) 
                        {
                            // We found part of a MAGIC_NUMBER header, so let's adjust the system to see if
                            // we receive the rest of it and the rest of the packet
                            mCurrentPacketSize = i;
                        }
                        else
                        {
                            // If we get here, then all the data received was junk, so reset the buffer.
                            mCurrentPacketSize = 0;
                        }
                    }
                }
            }
        }
    }

    return goodPacketFound;
}

bool Parser::isValidProtocolVersion(uint8_t protocolVersion)
{
    return protocolVersion == 1 || protocolVersion == 2;
}

bool Parser::isValidSubsystem(uint8_t subsysId)
{
    switch (subsysId)
    {
        case 1: // AOCS
        case 3: // CDH
        case 5: // COM
            return true;
        default:
            return false;
    }
}

bool Parser::isValidComponent(uint8_t subsysId, uint8_t compId)
{
    switch (subsysId)
    {
        case 1: // AOCS
            return compId == 20 || compId == 21 || compId == 22 || compId == 23 || compId == 30 || compId == 31;
        case 3: // CDH
            return compId == 0;
        case 5: // COM
            return compId == 1 || compId == 2 || compId == 10 || compId == 20;
        default:
            return false;
    }
}

int Parser::extractData(CompoTlm_t& tlm) const
{
    int rtn = -1; // Assume invalid date until we are sure the packet is valid

    // Header Byte 1
    uint8_t protocolVersion = (buffer[0] >> 5); // Get 3 MSB for Protocol Version
    uint8_t subsysId = (buffer[0] & 0x1F); // Get 5 LSB for Subsystem ID

    // Header Byte 2
    uint8_t compId = (buffer[1] >> 3); // Get 5 MSB to get Component ID
    uint8_t telemetryType = buffer[1] & 0x07; // Get 3 LSB for Telemetry Type

    if (mIsCompletePacket &&
        buffer[2] == RESERVED &&
        isValidProtocolVersion(protocolVersion) &&
        isValidSubsystem(subsysId) &&
        isValidComponent(subsysId, compId))
    {
        tlm.subsys_id = subsysId;
        tlm.compo_id = compId;

        // Although a regular if statement could have been used, I went
        // with a switch to represent the idea that there could be more
        // telemetry types in the future. Also, once is more than one type
        // I could make sense to switch on an enumeration instead.
        switch (telemetryType)
        {
            case 1: // Temperature
            {
               

                if constexpr (std::endian::native != std::endian::big)
                {
                    // System is little endian so swap the bytes to match
                    // system so the data is usable.
                    uint32_t tempAsInt = 0;
                    tempAsInt = (buffer[3] << 24);
                    tempAsInt |= (buffer[4] << 16);
                    tempAsInt |= (buffer[5] << 8);
                    tempAsInt |= buffer[6];

                    // Convert int32 to float
                    std::memcpy(&tlm.temperature, &tempAsInt, 4);
                }
                else
                {
                    // Convert int32 to float
                    std::memcpy(&tlm.temperature, &buffer[3], 4);
                }

                rtn = 0; // Set is valid data
                break;
            }
            default: // Unhandled Telemetry type
                break;
        }
    }

    return rtn;
}

uint8_t Parser::calculateChecksum(const std::span<uint8_t>& buffer)
{
    uint8_t checksum = 0;
    for (const auto& byte : buffer) 
    {
        checksum ^= byte;
    }

    return checksum;
}