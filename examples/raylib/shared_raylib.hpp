#pragma once

#include <raylib.h>
#include <raymath.h>

struct PlayerData {
    Vector2 position;
};

constexpr MessageType MSG_PLAYER_DATA = MSG_USER_BASE;
constexpr MessageType MSG_PLAYER_LEFT = MSG_USER_BASE+1;