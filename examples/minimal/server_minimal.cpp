#include <EasyNet/EasyNetServer.hpp>
#include "shared_minimal.hpp"

EasyNetServer net_server;

void OnRecieve(ENetEvent event);

int main(){
    std::cout << "Server running" << std::endl;
    EasyNetInit();
    net_server.CreateServer();
    net_server.SetOnReceive(OnRecieve);
    while (true) {
        net_server.Update();
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
