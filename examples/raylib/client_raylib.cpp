#include <EasyNet/EasyNetClient.hpp>
#include "shared_raylib.hpp"

#include <memory>
#include <iostream>
#include <math.h>

PlayerData player;
std::unordered_map<uint32_t, PlayerData> players;
EasyNetClient net_client;
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
    
    player.position.x = GetScreenWidth() / 2;
    player.position.y = GetScreenHeight() / 2;

    while (!WindowShouldClose()) {

        if (!connected) {
            if (IsKeyPressed(KEY_SPACE)) net_client.RequestConnectToServer();
            
            net_client.Update();

            BeginDrawing();
            DrawLoading();
            DrawLog(); 
            EndDrawing();
        }
        else {
            if (IsKeyPressed(KEY_SPACE)) net_client.RequestDisconnectFromServer();

            player.position.x += (IsKeyDown(KEY_D) - IsKeyDown(KEY_A))/50.f;
            player.position.y += (IsKeyDown(KEY_S) - IsKeyDown(KEY_W))/50.f;

            net_client.Update();
            net_client.SendPacket(
                CreatePacket<PlayerData>(MSG_PLAYER_DATA, player)
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
    MessageType msgType = ExtractMessageType(event.packet);
    switch (msgType) {
    case MSG_PLAYER_DATA:
        {
            auto&& [player, id] = ExtractDataWithID<PlayerData>(event.packet);
            players[id] = player;
        }
        break;
    case MSG_PLAYER_LEFT:
        auto id = ExtractDataIdOnly(event.packet);
        players.erase(id);
        break;
    }
}

bool Init() {
    if (!EasyNetInit()) {
        std::cerr << "Failed to initialize easy net" << std::endl;
        return false;
    }

    if (!net_client.CreateClient()) {
        std::cerr << "Failed to create net_client" << std::endl;
        return false;
    }
    else {
        std::cout << "net_client created" << std::endl;
    }

    net_client.SetOnConnect([](ENetEvent){connected=true;});
    net_client.SetOnDisconnect([](ENetEvent){connected=false;});
    net_client.SetOnReceive(OnRecieve);
    //EasyNetSetLogCallback(LogCallback);

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

    for (auto& [id, player] : players) {
        DrawCircleV(
            player.position, 50, RED
        );
    }

    DrawCircleV(
        player.position, 50, GREEN
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
