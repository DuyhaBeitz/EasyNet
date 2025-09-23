#include "EasyNetLog.hpp"

#include <iostream>

EasyNetLogLevel easy_net_log_level = Trace;
std::function<void(std::string)> easy_net_log_callback = 0;

void EasyNetSetLogLevel(EasyNetLogLevel log_level) {
    easy_net_log_level = log_level;
}

void EasyNetSetLogCallback(std::function<void(std::string)> callback) {
    easy_net_log_callback = callback;
}

void EasyNetDefaultLog(std::string msg) {std::cout << msg << std::endl;}

void EasyNetLog(EasyNetLogLevel level, std::string msg) {
    if (level >= easy_net_log_level) {
        std::string prefix;

        switch (level) {
            case Info: prefix  = "Info: "; break;
            case Warn: prefix  = "Warn: "; break;
            case Error: prefix = "Error: "; break;
            case Fatal: prefix = "Fatal: "; break;
            default: prefix = "Log : "; break;
        }

        if (easy_net_log_callback) {
            easy_net_log_callback(prefix + msg);
        } else {
            EasyNetDefaultLog(prefix + msg);
        }
    }
}