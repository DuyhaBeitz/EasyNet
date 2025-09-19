#include "ClientNetwork.hpp"
#include "shared_raylib.hpp"

#include <memory>
#include <iostream>
#include <math.h>

std::unordered_map<uint32_t, ClientData> m_clients_data;
ClientData m_local_client_data;
std::shared_ptr<Client> client;

bool Init();
void Draw();
void OnRecieve(ENetEvent event);

int main() {
    if (!Init()) {
        return 1;
    }

    client->ConnectToServer();
    client->SetOnReceive(OnRecieve);

    m_local_client_data.position.x = GetScreenWidth() / 2;
    m_local_client_data.position.y = GetScreenHeight() / 2;

    while (!WindowShouldClose()) {
        m_local_client_data.position.x += (IsKeyDown(KEY_D) - IsKeyDown(KEY_A))/50.f;
        m_local_client_data.position.y += (IsKeyDown(KEY_S) - IsKeyDown(KEY_W))/50.f;

        client->Update();
        client->SendPacket(
            CreatePacket<Vector2>(MSSG_VECTOR2, m_local_client_data.position)
        );

        Draw();
    }
    return 0;
}

void OnRecieve(ENetEvent event) {
    MessageType msgType = static_cast<MessageType>(event.packet->data[0]);
    switch (msgType) {
    case MSSG_VECTOR2:
        {
            auto&& [rec_position, id] = ExtractDataWithID<Vector2>(event.packet);
            m_clients_data[id].position = rec_position;
        }
        break;
    
    default:
        break;
    }
}

bool Init() {
    std::cout << "Starting" << std::endl;
    if (!EasyNetInit()) {
        std::cerr << "Failed to initialize easy net" << std::endl;
        return false;
    }
    client = std::make_shared<Client>();

    if (!client->CreateClient()) {
        std::cerr << "Failed to create client" << std::endl;
        return false;
    }
    else {
        std::cout << "client created" << std::endl;
    }

    InitWindow(1000, 1000, "Client");
    if (!IsWindowReady()) {
        std::cerr << "Failed to initialize window" << std::endl;
        return false;
    }
    SetWindowState(FLAG_WINDOW_TOPMOST);
    return true;
}

void Draw() {
    BeginDrawing();
    ClearBackground(LIGHTGRAY);

    for (auto& [id, client_data] : m_clients_data) {
        DrawCircleV(
            client_data.position, 50, RED
        );
    }

    DrawCircleV(
        m_local_client_data.position, 50, GREEN
    );

    EndDrawing();
}