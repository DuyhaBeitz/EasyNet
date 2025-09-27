#include <EasyNetServer.hpp>
#include "shared_minimal.hpp"
#include <memory>

std::unique_ptr<EasyNetServer> server;
bool running = true;

void OnRecieve(ENetEvent event);

int main(){
    std::cout << "Server running" << std::endl;
    EasyNetInit();
    server = std::make_unique<EasyNetServer>();
    server->CreateServer();
    server->SetOnReceive(OnRecieve);
    while (running) {
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
