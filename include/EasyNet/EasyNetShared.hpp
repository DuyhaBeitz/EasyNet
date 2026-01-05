#pragma once
#include "enet/enet.h"
#include "EasyNetLog.hpp"

#include <cstring> // For memcpy
#include <type_traits> // For static_assert

// data has to be trivially copyable (so no std::string)
// MessageType fits into 1 byte (2^8 = 256 more that enough?)

bool EasyNetInit();

// default message types
using MessageType = uint8_t;
constexpr MessageType SC_JOIN = 0;           // when new clients joins that's what others see
constexpr MessageType SC_DISCONNECT = 1;     // empy data (CreatePacketIDOnly)
constexpr MessageType SC_INITIAL_SELF = 2;   // server gives a new player some initial state
constexpr MessageType SC_INITIAL_OTHER = 3;  // server sends a new player initial state of other client
constexpr MessageType MSG_USER_BASE = 100;   // you can define you message types starting from there

MessageType ExtractMessageType(const ENetPacket* packet);

// typename is data you are sending
template <typename T> 
ENetPacket* CreatePacket(uint8_t msg_type, const T& data, enet_uint32 flags = ENET_PACKET_FLAG_RELIABLE);
template <typename T>
ENetPacket* CreatePacketWithID(uint8_t msg_type, uint32_t id, const T& data, enet_uint32 flags = ENET_PACKET_FLAG_RELIABLE);
ENetPacket* CreatePacketIDOnly(uint8_t msg_type, uint32_t id, enet_uint32 flags = ENET_PACKET_FLAG_RELIABLE);
ENetPacket* CreatePacketMsgTypeOnly(uint8_t msg_type, enet_uint32 flags = ENET_PACKET_FLAG_RELIABLE);

// typename is what data you are expecting to get
template <typename T>
T ExtractData(const ENetPacket* packet);
template <typename T>
std::pair<T, uint32_t> ExtractDataWithID(const ENetPacket* packet);
uint32_t ExtractDataIdOnly(const ENetPacket* packet);

// template definitions

template <typename T>
ENetPacket* CreatePacket(uint8_t msg_type, const T& data, enet_uint32 flags) {
    static_assert(std::is_trivially_copyable<T>::value, "CreatePacket: T must be a trivially copyable struct");

    size_t packetSize = sizeof(uint8_t) + sizeof(T);

    // Allocate buffer
    uint8_t* packetData = new uint8_t[packetSize];

    packetData[0] = static_cast<uint8_t>(msg_type);
    memcpy(&packetData[1], &data, sizeof(T));

    ENetPacket* packet = enet_packet_create(packetData, packetSize, flags);

    // Cleanup allocated buffer (ENet manages the packet now)
    delete[] packetData;

    return packet;
}

template <typename T>
ENetPacket* CreatePacketWithID(uint8_t msg_type, uint32_t id, const T& data, enet_uint32 flags) {
    static_assert(std::is_trivially_copyable<T>::value, "CreatePacketWithID: T must be a trivially copyable struct");

    // (1 byte for msg_type, 4 bytes for id, and the size of the struct)
    size_t packetSize = sizeof(uint8_t) + sizeof(uint32_t) + sizeof(T);

    uint8_t* packetData = new uint8_t[packetSize];

    packetData[0] = static_cast<uint8_t>(msg_type);

    // Set the peer id at the second position
    memcpy(&packetData[1], &id, sizeof(uint32_t));

    // Copy the struct data into the remaining space
    memcpy(&packetData[1 + sizeof(uint32_t)], &data, sizeof(T));

    ENetPacket* packet = enet_packet_create(packetData, packetSize, flags);

    // Clean up the allocated buffer (ENet manages the packet now)
    delete[] packetData;

    return packet;
}

template <typename T>
T ExtractData(const ENetPacket* packet) {
    static_assert(std::is_trivially_copyable<T>::value, "ExtractData: T must be a trivially copyable struct");

    // Ensure that packet size is enough for the data
    size_t dataSize = packet->dataLength - sizeof(uint8_t);  // Exclude the 1 byte for the message type

    // Validate that the remaining packet size matches the expected size for T
    if (dataSize != sizeof(T)) {
        EasyNetLog(Fatal, "Packet size mismatch with expected struct size.");
    }

    T data;
    memcpy(&data, &packet->data[1], sizeof(T));  // Copy the struct data from packet

    return data;
}

template <typename T>
std::pair<T, uint32_t> ExtractDataWithID(const ENetPacket* packet) {
    static_assert(std::is_trivially_copyable<T>::value, "T must be a trivially copyable struct");

    // Ensure the packet has the right size for the ID and struct data
    size_t dataSize = packet->dataLength - sizeof(uint32_t) - sizeof(uint8_t);  // Exclude 1 byte for msg_type and 4 bytes for ID

    // Validate the remaining packet size
    if (dataSize != sizeof(T)) {
        EasyNetLog(Fatal, "Packet size mismatch with expected struct size.");
    }

    uint32_t id;
    memcpy(&id, &packet->data[1], sizeof(uint32_t));  // Extract ID from packet

    T data;
    memcpy(&data, &packet->data[1 + sizeof(uint32_t)], sizeof(T));  // Extract struct data after ID

    return {data, id};  // Return a pair: struct data and the id
}