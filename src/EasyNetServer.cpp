#include "EasyNet/EasyNetServer.hpp"

bool EasyNetServer::CreateServer(const int port){
    m_address.host = ENET_HOST_ANY;
    //enet_address_set_host(&m_address, "0.0.0.0"));
    m_server_port = port;
    m_address.port = m_server_port;

    m_server = enet_host_create(&m_address, 32, 1, 0, 0);
    return bool(m_server);
}

void EasyNetServer::Update(){

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

void EasyNetServer::HandleConnect(ENetEvent event){
    char ip[64];
    if (enet_address_get_host_ip(&event.peer->address, ip, sizeof(ip)) == 0) {
        EasyNetLog(Trace, "Client connected: {}", ip);
    } else {
        EasyNetLog(Trace, "Failed to get IP address of connected client");
    }

    uint32_t new_id = enet_peer_get_id(event.peer);
    m_peers[new_id] = event.peer;

    if (m_customConnect) m_customConnect(event);
}

void EasyNetServer::HandleReceive(ENetEvent event){
    if (event.packet->dataLength < 1) {
        EasyNetLog(Error, "Received packet too small!");
        enet_packet_destroy(event.packet);
        return;
    }
    
    if (m_customReceive) m_customReceive(event);
    enet_packet_destroy(event.packet);
}

void EasyNetServer::HandleDisconnectTimeout(ENetEvent event){
    EasyNetLog(Info, "Timeout:");
    HandleDisconnect(event);
}

void EasyNetServer::HandleDisconnect(ENetEvent event){
    uint32_t id = enet_peer_get_id(event.peer);

    BroadcastExcept(id, CreatePacketIDOnly(SC_DISCONNECT, id));

    char ip[64];
    if (enet_address_get_host_ip(&event.peer->address, ip, sizeof(ip)) == 0) {
        EasyNetLog(Trace, "Client disconnected: {}", ip);
    } else {
        EasyNetLog(Trace, "Failed to get IP address of disconnected client");
    }
    
    if (!m_peers.erase(id)) {
        EasyNetLog(Error, "Disconnected client not found in map (ID: {})", id);
    }
    
    if (m_customDisconnect) m_customDisconnect(event);
}

bool EasyNetServer::SendTo(uint32_t id, ENetPacket *packet, enet_uint8 channel) {
    if (enet_peer_send(m_peers[id], channel, packet) < 0) {
        enet_packet_destroy(packet);
        return false;
    }
    return true;
}

/*
BROADCASTING:
Just looping through peers and calling SendTo() was wrong
since it increased internally used packet.referenceCount
so it's never freed, causing memory leak
e.g. the same packet shouldn't be sent multiple times
*/
void EasyNetServer::BroadcastExcept(uint32_t except_id, ENetPacket * packet, enet_uint8 channel){
    for (auto& [id, peer] : m_peers) {
        if (id == except_id) continue;

        ENetPacket* cloned_packet = enet_packet_create(packet->data, packet->dataLength, packet->flags);
        SendTo(id, cloned_packet, channel);
    }

    // the original packet isn't actually used
    enet_packet_destroy(packet);
}

void EasyNetServer::Broadcast(ENetPacket *packet, enet_uint8 channel){
    if (m_peers.empty()) {
        // if there are no peers, then the packet won't be added to destroy queue
        enet_packet_destroy(packet);
        return;
    }
    enet_host_broadcast(m_server, channel, packet);
}

void EasyNetServer::DisconnectClient(uint32_t id) {
    enet_peer_disconnect(m_peers[id], 0);
}

void EasyNetServer::DisconnectAll() {
    for (auto& [id, peer] : m_peers) {
        DisconnectClient(id);
    }
}

ENetHost *EasyNetServer::GetServer() {
    return m_server;
}
