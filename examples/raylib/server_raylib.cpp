#include <EasyNetServer.hpp>
#include "shared_raylib.hpp"

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
bool Init();
void OnConnect(ENetEvent event);
void OnRecieve(ENetEvent event);

int main(){
    if (!Init()) {
        return 1;
    }
    server->SetOnConnect(OnConnect);
    server->SetOnReceive(OnRecieve);

    while (running) {
        std::this_thread::sleep_for(10ms);
        server->Update();

        for (auto& [id, client_data] : server->GetClients()) {

            if (client_data.client_data.position.x < 0) {
                server->DisconnectClient(id);
            }
            else {
                server->BroadcastExcept(
                    id, 
                    CreatePacketWithID<Vector2>(MSSG_VECTOR2, id, client_data.client_data.position, ENET_PACKET_FLAG_UNRELIABLE_FRAGMENT),
                    0
                );
            }
        }
    }
    return 0;
}

bool Init() {
    std::cout << "Starting" << std::endl;
    
    if (!EasyNetInit()) {
        std::cerr << "Failed to initialize easy net" << std::endl;
        return false;
    }

    server = std::make_unique<MyServer>();
    if (!server->CreateServer()) {
        std::cerr << "Failed to create server" << std::endl;
        return false;
    }
    else {
        std::cout << "Server created" << std::endl;
    }
    
    return true;
}

void OnConnect(ENetEvent event) {
    std::cout << "Connected" << std::endl;
}

void OnRecieve(ENetEvent event) {
    MessageType msgType = static_cast<MessageType>(event.packet->data[0]);
    switch (msgType) {
    case MSSG_VECTOR2:
        {
            Vector2 rec_position = ExtractData<Vector2>(event.packet);
            uint32_t id = enet_peer_get_id(event.peer);
            server->GetClient(id)->client_data.position = rec_position;
        }
        break;
    
    default:
        break;
    }
}