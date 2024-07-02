#ifndef TLM_PARSER_H
#define TLM_PARSER_H

#include "tlm.h"

#include <array>
#include <cstdint>
#include <span>

/**
 * Class to parse a 
 */
class Parser
{
public:
    /**
     * Default constructor. Nothing done but standard initiation of member variables.
     */
    Parser();

    /**
     * Pulls in one byte at a time until a full packet is formed. A complete packet
     * is considered PACKET_SIZE number of bytes is received where the packet starts
     * with the MAGIC_HEADER and the checksum equals the XORing of all the bytes
     * between the MAGIC_HEADER and the Checksum.
     * @param byte One byte to load into the packet buffer
     * @return true when a valid packet is found and false otherwise
     */
    bool detectPkt(uint8_t byte);

    /**
     * Extracts data from an assumed valid buffer. Unexpected values may be returned if
     * the buffer contains invalid data.
     * @warning Do not call this member function if detectPkt has not returned true
     * @param[in,out] tlm An object representing a telemetry struct to populate
     * @return 0 if no errors occured during parsing of the buffer and -1 otherwise.
     * @note Potentially in the future other error types can be returned depending on the
     * error that occured.
     */
    int extractData(CompoTlm_t& tlm) const;

private:
    static constexpr std::array<uint8_t, 4> MAGIC_HEADER = { 0xAB, 0xBA, 0xCF, 0xFC };
    static constexpr uint8_t RESERVED = 0xFF;
    static constexpr uint8_t PACKET_SIZE = 12;
    static constexpr uint8_t MAGIC_HEADER_SIZE = 4;
    static constexpr uint8_t HEADER_PAYLOAD_SIZE = 7; // Size of Secondary Header, Reserved, and Payload
    static constexpr uint8_t CHECKSUM_SIZE = 1;

    static bool isValidProtocolVersion(uint8_t protocolVersion);
    static bool isValidSubsystem(uint8_t subsysId);
    static bool isValidComponent(uint8_t subsysId, uint8_t compId);

    /**
     * Given a buffer of bytes, this member function will XOR all the bytes together
     * and return the value as the checksum for this buffer.
     * @param buffer A span of bytes to calculate the checksum of
     * @return The buffer's checksum
     */
    static uint8_t calculateChecksum(const std::span<uint8_t>& buffer);

    /** Array to hold the packets data excluding the magic number */
    std::array<uint8_t, HEADER_PAYLOAD_SIZE + CHECKSUM_SIZE> buffer;

    /** Variable to maintain how many bytes are currently stored in our buffer */
    uint8_t mCurrentPacketSize;

    /** Holds if the current packet held is a complete packet and the checksum is correct. */
    bool mIsCompletePacket;
};

#endif // TLM_PARSER_H
