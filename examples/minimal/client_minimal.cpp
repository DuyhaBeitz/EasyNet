#include "EasyNetClient.hpp"
#include "shared_minimal.hpp"
#include <memory>
#include <iostream>

std::unordered_map<uint32_t, ClientData> m_clients_data;
ClientData m_local_client_data;
std::shared_ptr<Client> client;

int main() {
    EasyNetInit();
    client = std::make_shared<Client>();
    client->CreateClient();
    while (!client->ConnectToServer());

    TextPacketData text_packet("Hello from client!");
    client->SendPacket(
        CreatePacket(
            MSSG_TEXT,
            text_packet
        )
    );

    while (true) {
        client->Update();
    }
    return 0;
}