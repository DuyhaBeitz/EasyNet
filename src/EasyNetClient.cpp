#include "EasyNetClient.hpp"
#include <iostream>

bool Client::CreateClient(){
    m_client = enet_host_create(0, 1, 1, 0, 0);
    return bool(m_client);
}

bool Client::ConnectToServer(std::string server_ip, int server_port){
    strcpy(m_server_ip, server_ip.c_str());
    m_server_port = server_port;

    std::cout << "Connecting to server" << std::endl;
    if (m_client) {
        enet_address_set_host(&m_address, m_server_ip);
        m_address.port = m_server_port;
        m_peer = enet_host_connect(m_client, &m_address, 2, 0);

        if (m_peer) {
            std::cout << "Created peer" << std::endl;
            ENetEvent event;
            if (enet_host_service(m_client, &event, 1000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT){
                HandleConnect(event);
                return true;
            }
            else {
                std::cerr << "Failed to connect!" << std::endl;
                enet_peer_reset(m_peer);
                m_peer = nullptr;
            }
        }
        else {
            std::cerr << "Failed to connect!" << std::endl;
        }
    }
    else {
        std::cerr << "Could not connect because client is null" << std::endl;
    }

    return false;
}

void Client::DisconnectFromServer(){
    if (m_peer){
        enet_peer_disconnect(m_peer, 0);
    
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
}



void Client::Update(){
    ENetEvent event;
    while (enet_host_service(m_client, &event, 0) > 0){
        // NO CASE FOR CONNECTED, BECAUSE IT'S HANDLED IN connect_to_server FUNCTION !
        // FOR DISCONNECTIONS IT'S DIFFERENT, SINCE DISCONNECTION MAY HAPPEN WITHOUT CALLING disconnect_from_server
        switch (event.type){

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
    std::cout << "Successfully connected!" << std::endl;
    if (m_customConnect) m_customConnect(event);
}

void Client::HandleDisconnectTimeout(ENetEvent event){
    std::cout << "Timeout" << std::endl;
    HandleDisconnect(event);
}

void Client::HandleDisconnect(ENetEvent event){
    std::cout << "Disconnected" << std::endl;
    if (m_customDisconnect) m_customDisconnect(event);
}


void Client::HandleReceive(ENetEvent event){
    if (m_customReceive) m_customReceive(event);
}
