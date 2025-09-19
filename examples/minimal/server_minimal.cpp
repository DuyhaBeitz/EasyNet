#include <ServerNetworking.hpp>
#include "shared_minimal.hpp"

#include <memory>
#include <chrono>
#include <thread>

// user types
using namespace std::chrono_literals;
typedef Server<ClientData> MyServer;

// variables
std::unique_ptr<MyServer> server;
bool running = true;

// functions
void OnRecieve(ENetEvent event);

int main(){
    std::cout << "Starting" << std::endl;
    
    if (!EasyNetInit()) {
        std::cerr << "Failed to initialize easy net" << std::endl;
        return 1;
    }

    server = std::make_unique<MyServer>();
    if (!server->CreateServer()) {
        std::cerr << "Failed to create server" << std::endl;
        return 1;
    }
    else {
        std::cout << "Server created" << std::endl;
    }

    server->SetOnReceive(OnRecieve);

    while (running) {
        std::this_thread::sleep_for(10ms);
        server->Update();
    }
    return 0;
}

void OnRecieve(ENetEvent event) {
    MessageType msgType = static_cast<MessageType>(event.packet->data[0]);
    switch (msgType) {
    case MSSG_TEXT:
        {
            auto text_packet = ExtractData<TextPacketData>(event.packet);
            std::cout << text_packet.text << std::endl;
        }
        break;
    
    default:
        break;
    }
}
