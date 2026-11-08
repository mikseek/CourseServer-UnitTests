/** @file @brief Разбор параметров командной строки сервера. */
#pragma once
#include "net.h"
#include <cstdint>
#include <stdexcept>
#include <string>
namespace course {
struct ServerOptions {
    std::string database = "./clients.conf";
    std::string log = "./server.log";
    uint32_t port = 33333;
    bool help = false;
};
/** @brief Разбирает аргументы запуска без запуска сервера.
 * @return Настройки сервера или признак вывода справки.
 * @throw std::runtime_error При неизвестном параметре, отсутствии значения или неверном порте.
 */
inline ServerOptions parse_options(int argc, const char* const argv[]) {
    ServerOptions o;
    if (argc <= 1) {o.help = true; return o;}
    // Справка имеет приоритет независимо от порядка остальных параметров.
    for (int i=1; i<argc; ++i) {
        const std::string a=argv[i];
        if (a=="-h" || a=="--help") {o.help = true; return o;}
    }
    for (int i=1; i<argc; ++i) {
        const std::string a=argv[i];
        if (a=="--run") continue;
        if (a!="-b" && a!="--database" && a!="-l" && a!="--log" && a!="-p" && a!="--port")
            throw std::runtime_error("unknown option: "+a);
        if (++i==argc) throw std::runtime_error("missing option value: "+a);
        if (a=="-b" || a=="--database") o.database=argv[i];
        else if (a=="-l" || a=="--log") o.log=argv[i];
        else {
            o.port=parse_number(argv[i],65535);
            if (!o.port) throw std::runtime_error("port cannot be zero");
        }
    }
    return o;
}
}
