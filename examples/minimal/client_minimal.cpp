#include "EasyNetClient.hpp"
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