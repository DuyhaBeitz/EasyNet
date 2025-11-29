#include <EasyNet/EasyNetClient.hpp>
#include "shared_minimal.hpp"

EasyNetClient net_client;

int main() {
    EasyNetInit();
    net_client.CreateClient();
    while (!net_client.ConnectToServer("127.0.0.1", 7777));

    TextPacketData text_packet("Hello from net_client!");
    net_client.SendPacket(
        CreatePacket(
            MSG_TEXT,
            text_packet
        )
    );

    net_client.Update();
    net_client.DisconnectFromServer();

    return 0;
}