# EasyNet
EasyNet is a C++ wrapper for enet.h, specifically this fork of it: https://github.com/zpl-c/enet.

It allows the programmer to skip all of the common enet.h initialization/event handling stuff and makes it easier to send and extract received data.
EasyNet introduces high level classes for server and client, but it still exposes ENet types.

Each packet has a message type, like MSG_PLAYER_JOINED or MSG_HELLO.
There are 3 types of packets, each corresponding to two functions, for creating such packet, and extracting data from it:
- Data only
- Data + id associated with the peer (e.g. player id=3 moved to loc={1, 0})
- id only (e.g. MSG_PLAYER_LEFT id=3)

# Example client
```
#include <EasyNet/EasyNetClient.hpp>
#include "shared_minimal.hpp"

EasyNetClient client;

int main() {
    EasyNetInit();
    client.CreateClient();
    while (!client.ConnectToServer("127.0.0.1", 7777));

    TextPacketData text_packet("Hello from client!");
    client.SendPacket(
        CreatePacket(
            MSG_TEXT,
            text_packet
        )
    );

    client.Update();
    client.DisconnectFromServer();

    return 0;
}
```


# Example server
```
#include <EasyNet/EasyNetServer.hpp>
#include "shared_minimal.hpp"

EasyNetServer server;

void OnRecieve(ENetEvent event);

int main(){
    std::cout << "Server running" << std::endl;
    EasyNetInit();
    server.CreateServer();
    server.SetOnReceive(OnRecieve);
    while (true) {
        server.Update();
    }
    return 0;
}

void OnRecieve(ENetEvent event) {
    MessageType msgType = ExtractMessageType(event.packet);
    switch (msgType) {
    case MSG_TEXT:
        {
            auto text_packet = ExtractData<TextPacketData>(event.packet);
            std::cout << text_packet.text << std::endl;
        }
        break;
    
    default:
        break;
    }
}
```
# Example shared code
```
#pragma once
#include <EasyNet/EasyNetShared.hpp>

constexpr MessageType MSG_TEXT = MSG_USER_BASE;

struct TextPacketData {
    char text[64] = {};
    TextPacketData(const char* str) { 
        std::strncpy(text, str, sizeof(text));
        text[sizeof(text)-1] = '\0';
    }
    TextPacketData() = default; // needed for packet data, because before copying the data, the lvalue is declared
};
```
# Building & installing
EasyNet is a cmake project, you can build it and install or use fetch_content
