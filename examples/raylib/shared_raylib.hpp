// #pragma once

// #include <raylib.h>
// #include <raymath.h>

// struct ClientData {
//     Vector2 position;
// };

// constexpr MessageType MSG_TEXT = MSG_USER_BASE;
// constexpr MessageType MSG_VECTOR2 = MSG_USER_BASE+1;

// struct TextPacketData {
//     char text[64] = {};
//     TextPacketData(const char* str) { 
//         std::strncpy(text, str, sizeof(text));
//         text[sizeof(text)-1] = '\0';
//     }
//     TextPacketData() = default; // needed for packet data, because before copying the data, the lvalue is declared
// };