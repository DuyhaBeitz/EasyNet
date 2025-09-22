#define ENET_IMPLEMENTATION
#include "EasyNetShared.hpp"

bool EasyNetInit() {
    if (enet_initialize() != 0) {
        return false;
    }
    atexit (enet_deinitialize);
    return true;
}

ENetPacket *CreatePacketIDOnly(uint8_t msg_type, uint32_t id, enet_uint32 flags)
{
    size_t packetSize = sizeof(uint8_t) + sizeof(uint32_t);

    uint8_t* packetData = new uint8_t[packetSize];

    packetData[0] = static_cast<uint8_t>(msg_type);

    // Set the peer id at the second position
    memcpy(&packetData[1], &id, sizeof(uint32_t));

    ENetPacket* packet = enet_packet_create(packetData, packetSize, flags);

    // Clean up the allocated buffer (ENet manages the packet now)
    delete[] packetData;

    return packet;
}
