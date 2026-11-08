/** @file @brief Запись ошибок в журнал с отметкой времени и критичностью. */
#pragma once
#include <fstream>
#include <ctime>
#include <iomanip>
#include <stdexcept>
#include <string>
namespace course {
/** @brief Сохраняет ошибки сервера в текстовом файле. */
class Logger {
    std::ofstream out;
public:
    explicit Logger(const std::string& path):out(path,std::ios::app){if(!out)throw std::runtime_error("cannot open log: "+path);}
    void write(bool critical,const std::string& message) {
        const auto now=std::time(nullptr);auto* tm=std::localtime(&now);
        out<<std::put_time(tm,"%Y-%m-%d %H:%M:%S")<<' '<<(critical?"CRITICAL":"NONCRITICAL")<<' '<<message<<std::endl;
        if(!out)throw std::runtime_error("log write failed");
    }
};
}
