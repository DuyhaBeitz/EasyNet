#include <EasyNetServer.hpp>
#include "shared_raylib.hpp"

#include <memory>
#include <chrono>
#include <thread>

// user types
using namespace std::chrono_literals;

// variables
EasyNetServer server;
std::unordered_map<uint32_t, PlayerData> players;

// functions
bool Init();
void OnConnect(ENetEvent event);
void OnRecieve(ENetEvent event);
void OnConnect(ENetEvent event);
void OnDisconnect(ENetEvent event);

int main(){
    if (!Init()) {
        return 1;
    }
    server.SetOnConnect(OnConnect);
    server.SetOnDisconnect(OnDisconnect);
    server.SetOnReceive(OnRecieve);

    while (true) {
        std::this_thread::sleep_for(10ms);
        server.Update();

        for (auto& [id, player] : players) {

            if (player.position.x < 0) {
                server.DisconnectClient(id);
            }
            else {
                server.BroadcastExcept(
                    id, 
                    CreatePacketWithID<PlayerData>(MSG_PLAYER_DATA, id, player, ENET_PACKET_FLAG_UNRELIABLE_FRAGMENT)
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

    if (!server.CreateServer()) {
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

void OnDisconnect(ENetEvent event) {
    uint32_t id = enet_peer_get_id(event.peer);
    players.erase(id);
    server.Broadcast(CreatePacketIDOnly(MSG_PLAYER_LEFT, id));
    std::cout << "Sent\n";
}

void OnRecieve(ENetEvent event) {
    MessageType msgType = ExtractMessageType(event.packet);
    switch (msgType) {
    case MSG_PLAYER_DATA:
        {
            auto player = ExtractData<PlayerData>(event.packet);
            uint32_t id = enet_peer_get_id(event.peer);
            players[id] = player;
        }
        break;
    
    default:
        break;
    }
}