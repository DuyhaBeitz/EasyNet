#include "EasyNetClient.hpp"
#include "shared_minimal.hpp"
#include <memory>
#include <iostream>

std::shared_ptr<EasyNetClient> client;

int main() {
    EasyNetInit();
    client = std::make_shared<EasyNetClient>();
    client->CreateClient();
    while (!client->ConnectToServer());

    TextPacketData text_packet("Hello from client!");
    client->SendPacket(
        CreatePacket(
            MSSG_TEXT,
            text_packet
        )
    );

    client->Update();
    client->DisconnectFromServer();

    return 0;
}