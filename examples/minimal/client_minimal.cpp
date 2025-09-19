#include "ClientNetwork.hpp"
#include "shared_minimal.hpp"

#include <memory>
#include <iostream>

std::unordered_map<uint32_t, ClientData> m_clients_data;
ClientData m_local_client_data;
std::shared_ptr<Client> client;
bool running = true;

int main() {
    std::cout << "Starting" << std::endl;
    if (!EasyNetInit()) {
        std::cerr << "Failed to initialize easy net" << std::endl;
        return 1;
    }

    client = std::make_shared<Client>();

    if (!client->CreateClient()) {
        std::cerr << "Failed to create client" << std::endl;
        return 1;
    }
    else {
        std::cout << "Server created" << std::endl;
    }

    client->ConnectToServer();

    TextPacketData text_packet("Hello from client!");
    client->SendPacket(
        CreatePacket(
            MSSG_TEXT,
            text_packet
        )
    );

    while (running) {
        client->Update();
    }
    return 0;
}