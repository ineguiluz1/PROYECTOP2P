#ifndef LOGFILE_H
#define LOGFILE_H
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <ctime>

enum LogLevel { INFO, WARNING, FATAL };

class Log{
    public:
        Log(const std::string& filename);
        ~Log();
        void writeLog(LogLevel level, const std::string& message);
    private:
        std::ofstream logFile;
        std::string getTimestamp();
        std::string levelToString(LogLevel level);
};

#endif