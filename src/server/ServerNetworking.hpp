#pragma once

#include "Shared.hpp"

#include <iostream>
#include <unordered_map>
#include <functional>

template<typename T>
class Server {
private:
    struct Client{
        T client_data = {};
        ENetPeer* peer;
    };

    // Enet stuff
    const int m_server_port = 7777;
    ENetAddress m_address;
    ENetHost *m_server = nullptr;

    void HandleConnect(ENetEvent event);
    void HandleDisconnect(ENetEvent event);
    void HandleDisconnectTimeout(ENetEvent event);
    void HandleReceive(ENetEvent event);

    std::function<void(ENetEvent event)> m_customConnect = 0;
    std::function<void(ENetEvent event)> m_customDisconnect = 0;
    std::function<void(ENetEvent event)> m_customReceive = 0;

    std::unordered_map<uint32_t, Client> m_clients;

public:
    bool CreateServer();
    void Update();

    void SetOnConnect(std::function<void(ENetEvent event)> OnConnect) { m_customConnect = OnConnect; }
    void SetOnDisconnect(std::function<void(ENetEvent event)> OnDisconnect) { m_customDisconnect = OnDisconnect; }
    void SetOnReceive(std::function<void(ENetEvent event)> OnReceive) { m_customReceive = OnReceive; }

    int GetClientCount() { return m_clients.size(); }
    std::unordered_map<uint32_t, Client>& GetClients() { return m_clients; }
    Client* GetClient(uint32_t id) { return &m_clients[id]; }

    void SendTo(uint32_t id, ENetPacket* packet, enet_uint8 channel = 0);
    void Broadcast(ENetPacket* packet, enet_uint8 channel = 0);
    void BroadcastExcept(uint32_t except_id, ENetPacket* packet, enet_uint8 channel = 0);
};

template<typename T>
bool Server<T>::CreateServer(){
    //m_address.host = ENET_HOST_ANY;
    enet_address_set_host(&m_address, "127.0.0.1");
    m_address.port = m_server_port;

    m_server = enet_host_create(&m_address, 32, 1, 0, 0);
    return bool(m_server);
}

template<typename T>
void Server<T>::Update(){

    ENetEvent event;
    while (enet_host_service(m_server, &event, 0) > 0){
        switch (event.type){
            
            case ENET_EVENT_TYPE_NONE:
                break;
    
            case ENET_EVENT_TYPE_CONNECT:
                HandleConnect(event);
                break;

            case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT:
                HandleDisconnectTimeout(event);
                break;

            case ENET_EVENT_TYPE_DISCONNECT:
                HandleDisconnect(event);
                break;
    
            case ENET_EVENT_TYPE_RECEIVE:
                HandleReceive(event);
                break;
        }
    }
    enet_host_flush(m_server);
}


template<typename T>
void Server<T>::HandleConnect(ENetEvent event){
    char new_connection_ip[64];
    if (enet_address_get_host_ip(&event.peer->address, new_connection_ip, sizeof(new_connection_ip)) == 0) {
        std::cout << "Client connected: " << new_connection_ip << std::endl;
    } else {
        std::cerr << "Failed to get IP address of connected client\n";
    }

    uint32_t new_id = enet_peer_get_id(event.peer);
    m_clients[new_id].peer = event.peer;
    m_clients[new_id].client_data = {};

    if (m_clients.size() > 1){
        // tell other clients about a new connection
        BroadcastExcept(new_id, CreatePacketWithID<T>(SC_JOIN, new_id, m_clients[new_id].client_data));
    }
    
    // tell the new client what his initial state is
    enet_peer_send(event.peer, 0, CreatePacket<T>(SC_INITIAL_SELF, m_clients[new_id].client_data));
    // tell the new client what the initial states of others are
    for (auto& [id, client] : m_clients){
        if (id != new_id){
            enet_peer_send(event.peer, 0, CreatePacketWithID<T>(SC_INITIAL_OTHER, id, client.client_data));
        }
    }
    
    if (m_customConnect) m_customConnect(event);
}

template<typename T>
void Server<T>::HandleReceive(ENetEvent event){
    if (event.packet->dataLength < 1) {
        std::cerr << "Received packet too small!" << std::endl;
        enet_packet_destroy(event.packet);
        return;
    }
    
    if (m_customReceive) m_customReceive(event);
    enet_packet_destroy(event.packet);
}

template<typename T>
void Server<T>::HandleDisconnectTimeout(ENetEvent event){
    std::cout << "Timeout: ";
    HandleDisconnect(event);
}

template<typename T>
void Server<T>::HandleDisconnect(ENetEvent event){
    uint32_t id = enet_peer_get_id(event.peer);

    BroadcastExcept(id, CreatePacketIDOnly(SC_DISCONNECT, id));

    if (m_clients.erase(id)) {
        std::cout << "Client disconnected (ID: " << id << ")\n";
    } else {
        std::cerr << "Disconnected client not found in map (ID: " << id << ")\n";
    }
    
    if (m_customDisconnect) m_customDisconnect(event);
}

template <typename T>
inline void Server<T>::SendTo(uint32_t id, ENetPacket *packet, enet_uint8 channel) {
    enet_peer_send(m_clients[id].peer, channel, packet);
}

template<typename T>
void Server<T>::BroadcastExcept(uint32_t except_id, ENetPacket * packet, enet_uint8 channel){
    for (auto&& entry : m_clients){
        if (entry.first != except_id){
            SendTo(entry.first, packet, channel);
        }
    }
}

template <typename T>
inline void Server<T>::Broadcast(ENetPacket *packet, enet_uint8 channel)
{
    for (auto&& entry : m_clients){
        SendTo(entry.first, packet, channel);
    }
}
