#define CATCH_CONFIG_MAIN

#include <catch.hpp>

#include "tlm_parser.h"

#include <bit>
#include <cstring>
#include <vector>

// Function to create the Secondary Header with Reserved field
static std::vector<uint8_t> createSecondaryHeader(uint8_t protocolVersion, uint8_t subsysId, uint8_t compId, 
    uint8_t telemetryType) {
    std::vector<uint8_t> header;
    // Upper 3 bits (Protocol Version) | Lower 5 bits (Subsystem ID)
    header.push_back(static_cast<uint8_t>((protocolVersion << 5) | (subsysId & 0x1F)));
    // Upper 5 bits (Component ID) | Lower 3 bits (Telemetry type)
    header.push_back((static_cast<uint8_t>(compId << 3) | (telemetryType & 0x07)));
    // Reserved field
    header.push_back(0xFF);
    return header;
}

// Function to construct a complete packet
static std::vector<uint8_t> constructPacket(const std::vector<uint8_t>& secondaryHeader, float payload) {
    std::vector<uint8_t> packet = { 0xAB, 0xBA, 0xCF, 0xFC }; // Magic header
    packet.insert(packet.end(), secondaryHeader.begin(), secondaryHeader.end());

    // Add payload
    uint8_t payloadBytes[4];
    std::memcpy(payloadBytes, &payload, 4);
    packet.insert(packet.end(), payloadBytes, payloadBytes + 4);

    // Calculate checksum for all bytes between magic header and checksum byte
    uint8_t checksum = 0;
    for (size_t i = 4; i < packet.size(); ++i) {
        checksum ^= packet[i];
    }
    packet.push_back(checksum);

    return packet;
}

// Function to simulate filling the parser with a packet
static bool fillParserWithPacket(Parser& parser, const std::vector<uint8_t>& packet) {
    bool result = false;
    for (const uint8_t byte : packet) {
        result = parser.detectPkt(byte);
    }
    return result;
}

// Function to swap the endianness of a float
static float reverseFloat(const float inFloat)
{
    float retVal;
    char* floatToConvert = (char*)&inFloat;
    char* returnFloat = (char*)&retVal;

    // swap the bytes into a temporary buffer
    returnFloat[0] = floatToConvert[3];
    returnFloat[1] = floatToConvert[2];
    returnFloat[2] = floatToConvert[1];
    returnFloat[3] = floatToConvert[0];

    return retVal;
}

// Generates a random float for the payload as NativeEndian. Only generate
// this once so that it's easier to debug if one particular payload causes
// an issue.
static float getPayloadNative()
{
    // Create a random device and seed the random number generator
    static std::random_device rd;
    static std::mt19937 gen(rd());

    // Define the range for the random float
    static std::uniform_real_distribution<> dis(0.0, 1.0);

    // Generate a random float in the specified range
    static float randomFloat = static_cast<float>(dis(gen));

    return randomFloat;
}

// Generates a random float for the payload as BigEndian. Only generate
// this once so that it's easier to debug if one particular payload causes
// an issue.
static float getPayloadBigEndian()
{
    if constexpr (std::endian::native == std::endian::big)
    {
        return getPayloadNative();
    }
    else
    {
        return reverseFloat(getPayloadNative());
    }
}

static const std::vector<uint8_t> protocolVersions = { 1, 2 }; 
static const std::vector<uint8_t> subsysIds = { 1, 3, 5 }; // AOCS, CDH, COM
static const std::vector<std::vector<uint8_t>> compIds = 
    {
        {20, 21, 22, 23, 30, 31}, // AOCS
        {0},                      // CDH
        {1, 2, 10, 20}            // COM
    };
static constexpr uint8_t telemetryType = 1; // Only one telemetry type specified

TEST_CASE("Parser detects packets correctly", "[detectPkt]") {
    Parser parser;

    // DYNAMIC SECTION
    {
        // Iterate through all valid combinations of protocol, subsystem ids, and component ids
        for (uint8_t protocolVersion : protocolVersions)
        {
            for (size_t subsysIdsIdx = 0; subsysIdsIdx < subsysIds.size(); ++subsysIdsIdx)
            {
                for (uint8_t compId : compIds[subsysIdsIdx])
                {
                    std::vector<uint8_t> secondaryHeader = 
                        createSecondaryHeader(protocolVersion, subsysIds[subsysIdsIdx], compId, telemetryType);

                    DYNAMIC_SECTION("Valid packet detection :\n"
                        << "Protocol Version | Subsystem ID | Component ID | Telemetry Type | Payload\n"
                        << static_cast<int>(protocolVersion) << " | "
                        << static_cast<int>(subsysIds[subsysIdsIdx]) << " | "
                        << static_cast<int>(compId) << " | "
                        << static_cast<int>(telemetryType) << " | "
                        << getPayloadNative())
                    {
                        std::vector<uint8_t> packet = constructPacket(secondaryHeader, getPayloadBigEndian());
                        CHECK(fillParserWithPacket(parser, packet) == true);
                    }
                }
            }
        }
    }

    SECTION("Packet with invalid Checksum")
    {
        auto header = createSecondaryHeader(1, 1, 20, 1);
        auto packet = constructPacket(header, getPayloadBigEndian());
        packet.back() = 0x00; // Corrupt the checksum
        bool result = fillParserWithPacket(parser, packet);
        CHECK(result == false);
    }

    SECTION("Packet with invalid Magic Header")
    {
        auto header = createSecondaryHeader(1, 1, 20, 1);
        auto packet = constructPacket(header, getPayloadBigEndian());
        packet[0] = 0xAA; // Corrupt the magic header
        bool result = fillParserWithPacket(parser, packet);
        CHECK(result == false);
    }

    SECTION("Packet with invalid Protocol Version")
    {
        // Invalid protocol version
        auto header = createSecondaryHeader(3, 1, 20, 1); 
        auto packet = constructPacket(header, getPayloadBigEndian());
        bool result = fillParserWithPacket(parser, packet);
        CHECK(result == true); // This is not checked until extractData so result will be valid
    }

    SECTION("Packet with invalid Subsystem ID")
    {
        // Invalid subsystem ID
        auto header = createSecondaryHeader(1, 6, 20, 1);
        auto packet = constructPacket(header, getPayloadBigEndian());
        bool result = fillParserWithPacket(parser, packet);
        CHECK(result == true); // This is not checked until extractData so result will be valid
    }

    SECTION("Packet with invalid Component ID")
    {
        // Invalid subsystem ID
        auto header = createSecondaryHeader(1, 1, 40, 1);
        auto packet = constructPacket(header, getPayloadBigEndian());
        bool result = fillParserWithPacket(parser, packet);
        CHECK(result == true); // This is not checked until extractData so result will be valid
    }

    SECTION("Packet with invalid Telemetry Type")
    {
        // Invalid subsystem ID
        auto header = createSecondaryHeader(1, 1, 20, 2);
        auto packet = constructPacket(header, getPayloadBigEndian());
        bool result = fillParserWithPacket(parser, packet);
        CHECK(result == true); // This is not checked until extractData so result will be valid
    }

    SECTION("Packet starting with spurious bytes")
    {
        auto header = createSecondaryHeader(1, 1, 20, 1);
        auto packet = constructPacket(header, getPayloadBigEndian());

        // Put some random bytes at the front of our message
        std::vector<uint8_t> newBytes = { 0x01, 0x02, 0x03 };
        packet.insert(packet.begin(), newBytes.begin(), newBytes.end());

        bool result = fillParserWithPacket(parser, packet);
        CHECK(result == true); // System should handle this properly and purge bad bytes
    }

    SECTION("Packet with lost inner bytes and complete MAGIC_HEADER can be found")
    {
        auto header = createSecondaryHeader(1, 1, 20, 1);
        auto packet = constructPacket(header, getPayloadBigEndian());
        auto packetCopy(packet);
        packet.resize(packetCopy.size() + 5);

        // Overwrite the end of the packet with valid message
        for (size_t i = 5, j = 0; j < packetCopy.size(); ++i, ++j) 
        {
            packet[i] = packetCopy[j];
        }

        bool result = fillParserWithPacket(parser, packet);
        CHECK(result == true); // System should handle this properly and purge bad bytes
    }

    SECTION("Packet with lost inner bytes and complete MAGIC_HEADER not initially found")
    {
        auto header = createSecondaryHeader(1, 1, 20, 1);
        auto packet = constructPacket(header, getPayloadBigEndian());
        auto packetCopy(packet);
        packet.resize(packetCopy.size() + 9);


        // Overwrite the end of the packet with valid message
        for (size_t i = 9, j = 0; j < packetCopy.size(); ++i, ++j)
        {
            packet[i] = packetCopy[j];
        }

        bool result = fillParserWithPacket(parser, packet);
        CHECK(result == true); // System should handle this properly and purge bad bytes
    }
}

TEST_CASE("Parser extracts data correctly", "[extractData]")
{
    Parser parser;
    CompoTlm_t telemetry;

    // DYNAMIC SECTION
    {
        // Iterate through all valid combinations of protocol, subsystem ids, and component ids
        for (uint8_t protocolVersion : protocolVersions)
        {
            for (size_t subsysIdsIdx = 0; subsysIdsIdx < subsysIds.size(); ++subsysIdsIdx)
            {
                for (uint8_t compId : compIds[subsysIdsIdx])
                {
                    std::vector<uint8_t> secondaryHeader =
                        createSecondaryHeader(protocolVersion, subsysIds[subsysIdsIdx], compId, telemetryType);

                    DYNAMIC_SECTION("Extract data from valid packet :\n"
                        << "Protocol Version | Subsystem ID | Component ID | Telemetry Type | Payload\n"
                        << static_cast<int>(protocolVersion) << " | "
                        << static_cast<int>(subsysIds[subsysIdsIdx]) << " | "
                        << static_cast<int>(compId) << " | "
                        << static_cast<int>(telemetryType) << " | "
                        << getPayloadNative())
                    {
                        std::vector<uint8_t> packet = constructPacket(secondaryHeader, getPayloadBigEndian());
                        bool result = fillParserWithPacket(parser, packet);
                        CHECK(result == true);

                        if (result)
                        {
                            int error = parser.extractData(telemetry);

                            CHECK(error == 0);
                            CHECK(telemetry.subsys_id == subsysIds[subsysIdsIdx]);
                            CHECK(telemetry.compo_id == compId);
                            CHECK(telemetry.temperature == Approx(getPayloadNative()));
                        }
                    }
                }
            }
        }
    }

    SECTION("Extract data from packet with invalid Checksum")
    {
        auto header = createSecondaryHeader(1, 1, 20, 1);
        auto packet = constructPacket(header, getPayloadBigEndian());
        packet.back() = 0x00; // Corrupt the checksum
        fillParserWithPacket(parser, packet);

        int result = parser.extractData(telemetry);

        REQUIRE(result != 0);
    }

    SECTION("Extract data from packet with invalid Magic Header")
    {
        auto header = createSecondaryHeader(1, 1, 20, 1);
        auto packet = constructPacket(header, getPayloadBigEndian());
        packet[0] = 0xAA; // Corrupt the magic header
        fillParserWithPacket(parser, packet);

        int result = parser.extractData(telemetry);

        CHECK(result != 0);
    }

    SECTION("Extract data from packet with invalid protocol version")
    {
        // Invalid protocol version
        auto header = createSecondaryHeader(3, 1, 20, 1);
        auto packet = constructPacket(header, 20.0f);
        fillParserWithPacket(parser, packet);

        int result = parser.extractData(telemetry);

        CHECK(result != 0);
    }

    SECTION("Extract data from packet with invalid subsystem ID")
    {
        // Invalid subsystem ID
        auto header = createSecondaryHeader(1, 6, 20, 1);
        auto packet = constructPacket(header, 20.0f);
        fillParserWithPacket(parser, packet);

        int result = parser.extractData(telemetry);

        CHECK(result != 0);
    }

    SECTION("Extract data from packet with invalid component ID")
    {
        // Invalid component ID
        auto header = createSecondaryHeader(1, 1, 40, 1);
        auto packet = constructPacket(header, 20.0f);
        fillParserWithPacket(parser, packet);

        int result = parser.extractData(telemetry);

        CHECK(result != 0);
    }

    SECTION("Extract data from packet with invalid telemetry type")
    {
        // Invalid telemetry type
        auto header = createSecondaryHeader(1, 1, 20, 2);
        auto packet = constructPacket(header, 20.0f);
        fillParserWithPacket(parser, packet);

        int result = parser.extractData(telemetry);

        CHECK(result != 0);
    }

    SECTION("Extract data from packet starting with spurious bytes")
    {
        auto header = createSecondaryHeader(1, 1, 20, 1);
        auto packet = constructPacket(header, getPayloadBigEndian());

        // Put some random bytes at the front of our message
        std::vector<uint8_t> newBytes = { 0x01, 0x02, 0x03 };
        packet.insert(packet.begin(), newBytes.begin(), newBytes.end());
        bool result = fillParserWithPacket(parser, packet);

        CHECK(result == true);
        if (result)
        {
            int error = parser.extractData(telemetry);

            // System should handle this properly and purge bad bytes
            CHECK(error == 0);
            CHECK(telemetry.subsys_id == 1);
            CHECK(telemetry.compo_id == 20);
            CHECK(telemetry.temperature == Approx(getPayloadNative()));
        }
    }

    SECTION("Extract packet with lost inner bytes and complete MAGIC_HEADER can be found")
    {
        auto header = createSecondaryHeader(1, 1, 20, 1);
        auto packet = constructPacket(header, getPayloadBigEndian());
        auto packetCopy(packet);
        packet.resize(packetCopy.size() + 5);

        // Overwrite the end of the packet with valid message
        for (size_t i = 5, j = 0; j < packetCopy.size(); ++i, ++j)
        {
            packet[i] = packetCopy[j];
        }

        bool result = fillParserWithPacket(parser, packet);
        CHECK(result == true); // System should handle this properly and purge bad bytes

        if (result)
        {
            int error = parser.extractData(telemetry);

            // System should handle this properly and purge bad bytes
            CHECK(error == 0);
            CHECK(telemetry.subsys_id == 1);
            CHECK(telemetry.compo_id == 20);
            CHECK(telemetry.temperature == Approx(getPayloadNative()));
        }
    }

    SECTION("Extract packet with lost inner bytes and complete MAGIC_HEADER not initially found")
    {
        auto header = createSecondaryHeader(1, 1, 20, 1);
        auto packet = constructPacket(header, getPayloadBigEndian());
        auto packetCopy(packet);
        packet.resize(packetCopy.size() + 9);


        // Overwrite the end of the packet with valid message
        for (size_t i = 9, j = 0; j < packetCopy.size(); ++i, ++j)
        {
            packet[i] = packetCopy[j];
        }

        bool result = fillParserWithPacket(parser, packet);
        CHECK(result == true); // System should handle this properly and purge bad bytes

        if (result)
        {
            int error = parser.extractData(telemetry);

            // System should handle this properly and purge bad bytes
            CHECK(error == 0);
            CHECK(telemetry.subsys_id == 1);
            CHECK(telemetry.compo_id == 20);
            CHECK(telemetry.temperature == Approx(getPayloadNative()));
        }
    }
}
