#pragma once

#include <functional>
#include <format>

enum EasyNetLogLevel {
    Trace = 0,
    Debug,
    Info,
    Warn,
    Error,
    Fatal,
    None
};

void EasyNetSetLogLevel(EasyNetLogLevel log_level);
void EasyNetSetLogCallback(std::function<void(std::string)> callback);

void EasyNetDefaultLog(std::string msg);
void EasyNetLog(EasyNetLogLevel level, std::string msg);

template<typename... Args>
void EasyNetLog(EasyNetLogLevel level, std::format_string<Args...> fmt, Args&&... args) {
    auto msg = std::format(fmt, std::forward<Args>(args)...);
    EasyNetLog(level, msg);
}