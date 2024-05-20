#include "logFile.h"
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <ctime>

Log::Log(const std::string& filename){
    logFile.open(filename, std::ios::app);
    if (!logFile.is_open()) {
        std::cerr << "No se pudo abrir el archivo de log." << std::endl;
    }
}

Log::~Log(){
    if (logFile.is_open()) {
        logFile.close();
    }
}

std::string Log::getTimestamp(){
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::string timestamp = std::ctime(&now_c);
    timestamp.pop_back();   // Elimina el caracter de nueva línea
    return timestamp;
}

std::string Log::levelToString(LogLevel level){
    switch (level) {
        case INFO:
            return "INFO";
        case WARNING:
            return "WARNING";
        case FATAL:
            return "ERROR";
        default:
            return "UNKNOWN";
    }
}

void Log::writeLog(LogLevel level, const std::string& message){
    if (logFile.is_open()) {
        logFile << getTimestamp() << " " << levelToString(level) << " " << message << std::endl;
    }
}