#include <EasyNetServer.hpp>
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
