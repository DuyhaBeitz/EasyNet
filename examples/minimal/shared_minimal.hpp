#pragma once

struct ClientData {
    const char* name;
};

constexpr MessageType MSSG_TEXT = MSG_USER_BASE;


struct TextPacketData {
    char text[64] = {};
    TextPacketData(const char* str) { 
        std::strncpy(text, str, sizeof(text));
        text[sizeof(text)-1] = '\0';
    }
    TextPacketData() = default; // needed for packet data, because before copying the data, the lvalue is declared
};