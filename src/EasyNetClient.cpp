#include "EasyNetClient.hpp"
#include <iostream>

bool Client::CreateClient(){
    m_client = enet_host_create(0, 1, 1, 0, 0);
    return bool(m_client);
}

bool Client::RequestConnectToServer(std::string server_ip, int server_port) {
    strcpy(m_server_ip, server_ip.c_str());
    m_server_port = server_port;

    EasyNetLog(Trace, "Connecting to server");

    if (m_client) {
        enet_address_set_host(&m_address, m_server_ip);
        m_address.port = m_server_port;
        m_peer = enet_host_connect(m_client, &m_address, 2, 0);

        if (m_peer) {
            EasyNetLog(Trace, "Created peer");
            return true;
        }
        else {
            EasyNetLog(Info, "Failed to create peer");
        }
    }
    else {
        EasyNetLog(Error, "Could not connect because client is null");
    }
    return false;
}

bool Client::ConnectToServer(std::string server_ip, int server_port){
    strcpy(m_server_ip, server_ip.c_str());
    m_server_port = server_port;

    if (RequestConnectToServer(server_ip, server_port)) {
        ENetEvent event;
        if (enet_host_service(m_client, &event, 1000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT){
            HandleConnect(event);
            return true;
        }
        else {
            EasyNetLog(Error, "Failed to connect! (peer doesn't respond)");
            enet_peer_reset(m_peer);
            m_peer = nullptr;
        }
    }

    return false;
}

void Client::RequestDisconnectFromServer() {
    if (m_peer){
        enet_peer_disconnect(m_peer, 0);
    }
}

void Client::DisconnectFromServer()
{
    RequestDisconnectFromServer();
    
    /* Allow up to 3 seconds for the disconnect to succeed
    * and drop any packets received packets.
    */
    ENetEvent event;
    while (enet_host_service(m_client, &event, 3000) > 0)
    {
        switch (event.type)
        {
        case ENET_EVENT_TYPE_RECEIVE:
            enet_packet_destroy(event.packet);
            break;
        case ENET_EVENT_TYPE_DISCONNECT:
            HandleDisconnect(event);
            return;
        default:
            break;
        }
    }
    /* We've arrived here, so the disconnect attempt didn't */
    /* succeed yet.  Force the connection down.             */
    enet_peer_reset(m_peer);
}

void Client::Update(){
    ENetEvent event;
    while (enet_host_service(m_client, &event, 0) > 0){
        switch (event.type){
            case ENET_EVENT_TYPE_CONNECT:
                HandleConnect(event);
                break;

            case ENET_EVENT_TYPE_NONE:
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
    enet_host_flush(GetClient());
}

void Client::SendPacket(ENetPacket* packet) {
    enet_peer_send(GetPeer(), 0, packet);
}

void Client::HandleConnect(ENetEvent event){
    EasyNetLog(Trace, "Successfully connected!");
    if (m_customConnect) m_customConnect(event);
}

void Client::HandleDisconnectTimeout(ENetEvent event){
    EasyNetLog(Trace, "Timeout:");
    HandleDisconnect(event);
}

void Client::HandleDisconnect(ENetEvent event){
    EasyNetLog(Trace, "Disconnected");
    if (m_customDisconnect) m_customDisconnect(event);
}


void Client::HandleReceive(ENetEvent event){
    if (m_customReceive) m_customReceive(event);
}
