#pragma once

#include "EasyNetShared.hpp"

#include <iostream>
#include <unordered_map>
#include <functional>

class EasyNetServer {
private:
    // Enet stuff
    int m_server_port;
    ENetAddress m_address;
    ENetHost *m_server = nullptr;

    void HandleConnect(ENetEvent event);
    void HandleDisconnect(ENetEvent event);
    void HandleDisconnectTimeout(ENetEvent event);
    void HandleReceive(ENetEvent event);

    std::function<void(ENetEvent event)> m_customConnect = 0;
    std::function<void(ENetEvent event)> m_customDisconnect = 0;
    std::function<void(ENetEvent event)> m_customReceive = 0;

    std::unordered_map<uint32_t, ENetPeer*> m_peers;

public:
    bool CreateServer(const int port = 7777);
    void Update();

    void SetOnConnect(std::function<void(ENetEvent event)> OnConnect) { m_customConnect = OnConnect; }
    void SetOnDisconnect(std::function<void(ENetEvent event)> OnDisconnect) { m_customDisconnect = OnDisconnect; }
    void SetOnReceive(std::function<void(ENetEvent event)> OnReceive) { m_customReceive = OnReceive; }

    int GetClientCount() { return m_peers.size(); }
    std::unordered_map<uint32_t, ENetPeer*>& GetPeers() { return m_peers; }
    ENetPeer* GetClient(uint32_t id) { return m_peers[id]; }

    bool SendTo(uint32_t id, ENetPacket* packet, enet_uint8 channel = 0);
    void Broadcast(ENetPacket* packet, enet_uint8 channel = 0);
    void BroadcastExcept(uint32_t except_id, ENetPacket* packet, enet_uint8 channel = 0);

    void DisconnectClient(uint32_t id);
    void DisconnectAll();
    ENetHost* GetServer();
};
