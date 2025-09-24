#pragma once

#include "EasyNetShared.hpp"
#include <unordered_map>
#include <functional>

class Client {
private:
    int m_server_port;
    char m_server_ip[64];

    ENetAddress m_address;
    ENetPeer *m_peer = nullptr; // client's peer is the server
    ENetHost *m_client = nullptr;

    void HandleConnect(ENetEvent event);
    void HandleDisconnect(ENetEvent event);
    void HandleDisconnectTimeout(ENetEvent event);
    void HandleReceive(ENetEvent event);

    std::function<void(ENetEvent event)> m_customConnect = 0;
    std::function<void(ENetEvent event)> m_customDisconnect = 0;
    std::function<void(ENetEvent event)> m_customReceive = 0;

public:
    bool CreateClient();
    bool RequestConnectToServer(std::string server_ip = "127.0.0.1", int server_port = 7777); // non-blocking function
    bool ConnectToServer(std::string server_ip = "127.0.0.1", int server_port = 7777); // blocking function, but it ensures the result
    void DisconnectFromServer();

    void Update();

    ENetPeer *GetPeer() { return m_peer; }
    ENetHost *GetClient() { return m_client; }
    
    void SetOnConnect(std::function<void(ENetEvent event)> OnConnect) { m_customConnect = OnConnect; }
    void SetOnDisconnect(std::function<void(ENetEvent event)> OnDisconnect) { m_customDisconnect = OnDisconnect; }
    void SetOnReceive(std::function<void(ENetEvent event)> OnReceive) { m_customReceive = OnReceive; }

    void SendPacket(ENetPacket* packet);
};