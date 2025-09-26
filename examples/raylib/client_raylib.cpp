#include "EasyNetClient.hpp"
#include "shared_raylib.hpp"

#include <memory>
#include <iostream>
#include <math.h>

std::unordered_map<uint32_t, ClientData> m_clients_data;
ClientData m_local_client_data;
std::shared_ptr<Client> client;
std::vector<std::string> logged_messages;

bool connected = false;

bool Init();
void DrawLoading();
void Draw();
void DrawLog();
void OnRecieve(ENetEvent event);
void LogCallback(std::string msg);

int main() {
    if (!Init()) {
        return 1;
    }
    
    m_local_client_data.position.x = GetScreenWidth() / 2;
    m_local_client_data.position.y = GetScreenHeight() / 2;

    while (!WindowShouldClose()) {

        if (!connected) {
            if (IsKeyPressed(KEY_SPACE)) client->RequestConnectToServer();
            
            client->Update();

            BeginDrawing();
            DrawLoading();
            DrawLog(); 
            EndDrawing();
        }
        else {
            if (IsKeyPressed(KEY_SPACE)) client->RequestDisconnectFromServer();

            m_local_client_data.position.x += (IsKeyDown(KEY_D) - IsKeyDown(KEY_A))/50.f;
            m_local_client_data.position.y += (IsKeyDown(KEY_S) - IsKeyDown(KEY_W))/50.f;

            client->Update();
            client->SendPacket(
                CreatePacket<Vector2>(MSSG_VECTOR2, m_local_client_data.position, ENET_PACKET_FLAG_UNRELIABLE_FRAGMENT)
            );
            BeginDrawing();
            Draw();
            DrawLog();
            EndDrawing();
        }
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

    client->SetOnConnect([](ENetEvent){connected=true;});
    client->SetOnDisconnect([](ENetEvent){connected=false;});
    client->SetOnReceive(OnRecieve);
    EasyNetSetLogCallback(LogCallback);

    InitWindow(1000, 1000, "Client");
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    if (!IsWindowReady()) {
        std::cerr << "Failed to initialize window" << std::endl;
        return false;
    }
    SetWindowState(FLAG_WINDOW_TOPMOST);
    return true;
}

void DrawLoading() {
    ClearBackground(DARKGRAY);
    int font_size = 64;
    const char* text = "Press space to connect";
    float spacing = 10;
    Vector2 size = MeasureTextEx(GetFontDefault(), text, font_size, spacing);
    DrawTextEx(GetFontDefault(), text, Vector2{float(GetScreenWidth()), float(GetScreenHeight())} - size, font_size, spacing, WHITE);
}

void Draw() {
    ClearBackground(LIGHTGRAY);

    for (auto& [id, client_data] : m_clients_data) {
        DrawCircleV(
            client_data.position, 50, RED
        );
    }

    DrawCircleV(
        m_local_client_data.position, 50, GREEN
    );
}

void DrawLog() {
    for (int i = 0; i < logged_messages.size(); i++) {
        int size = 64;
        DrawText(logged_messages[i].c_str(), 0, size*i, size, RED);
    }
}

void LogCallback(std::string msg) {
    logged_messages.push_back(msg);
    if (logged_messages.size() > 3) {
        logged_messages.pop_back();
    }
    std::cout << msg << std::endl;
}
