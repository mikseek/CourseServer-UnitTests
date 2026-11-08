#include "core.h"
#include "net.h"
#include "logger.h"
#include "interface.h"
#include <arpa/inet.h>
#include <csignal>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <algorithm>
namespace {
volatile std::sig_atomic_t stopped=0;
void stop(int){stopped=1;}

bool read_authentication(int fd,const std::map<std::string,std::string>& users) {
    std::string message;
    for(size_t i=0;i<184;++i) {
        char c;course::transfer(fd,&c,1,false);message+=c;
        bool pending=false;
        for(const auto& entry:users) {
            const auto& login=entry.first;
            const size_t prefix=std::min(message.size(),login.size());
            if(message.compare(0,prefix,login,0,prefix)!=0)continue;
            const size_t expected=login.size()+56;
            if(message.size()<expected)pending=true;
            else if(message.size()==expected&&course::authenticate(message,users))return true;
        }
        if(!pending)return false;
    }
    return false;
}
void usage(){std::cout<<"Usage: server --run [-b clients.conf] [-l server.log] [-p 33333]\n"
    "  -b, --database FILE  User database (default ./clients.conf)\n"
    "  -l, --log FILE       Error log (default ./server.log)\n"
    "  -p, --port PORT      TCP port 1..65535 (default 33333)\n"
    "  --run                Start with default settings\n"
    "  -h, --help           Show help\n";}
}
int main(int argc,char** argv) {
    using namespace course;
    std::signal(SIGPIPE,SIG_IGN);
    try {
        auto opts=parse_options(argc,const_cast<const char* const*>(argv));
        if (opts.help) {usage();return 0;}
        const std::string& db=opts.database;
        const std::string& log=opts.log;
        const uint32_t port=opts.port;
        Logger logger(log);
        try {
            auto users=load_users(db);
            Socket listener(::socket(AF_INET,SOCK_STREAM,0));int yes=1;
            if(setsockopt(listener.get(),SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(yes))<0)throw std::runtime_error("setsockopt failed");
            sockaddr_in addr{};addr.sin_family=AF_INET;addr.sin_addr.s_addr=htonl(INADDR_ANY);addr.sin_port=htons(uint16_t(port));
            if(bind(listener.get(),reinterpret_cast<sockaddr*>(&addr),sizeof(addr))<0||listen(listener.get(),32)<0)throw std::runtime_error(std::strerror(errno));
            struct sigaction sa{};sa.sa_handler=stop;sigemptyset(&sa.sa_mask);
            if(sigaction(SIGINT,&sa,nullptr)<0||sigaction(SIGTERM,&sa,nullptr)<0)throw std::runtime_error("sigaction failed");
            std::cout<<"Listening on TCP port "<<port<<std::endl;
            while(!stopped){
                fd_set set;FD_ZERO(&set);FD_SET(listener.get(),&set);timeval wait{1,0};
                int ready=select(listener.get()+1,&set,nullptr,nullptr,&wait);
                if(ready<0){if(errno==EINTR)continue;throw std::runtime_error(std::strerror(errno));}if(!ready)continue;
                sockaddr_in peer{};socklen_t len=sizeof(peer);int fd=accept(listener.get(),reinterpret_cast<sockaddr*>(&peer),&len);
                if(fd<0){if(errno==EINTR)continue;logger.write(false,"accept: "+std::string(std::strerror(errno)));continue;}
                Socket client(fd);std::string context="peer="+std::string(inet_ntoa(peer.sin_addr))+":"+std::to_string(ntohs(peer.sin_port));
                try {
                    set_timeout(fd,180);
                    bool valid=false;
                    try {valid=read_authentication(fd,users);}
                    catch(const std::exception&) {
                        try {send_text(fd,"ERR");}catch(const std::exception&) {}
                        throw;
                    }
                    if(!valid){send_text(fd,"ERR");logger.write(false,context+" authentication failed");continue;}
                    send_text(fd,"OK");uint32_t count=read_number(fd,4);
                    if(count>10000)throw std::runtime_error("vector count exceeds 10000");
                    // Binary response header required by specification: number of results.
                    write_number(fd,count,4);
                    uint64_t total=0;
                    for(uint32_t i=0;i<count&&!stopped;++i){uint32_t size=read_number(fd,4);total+=size;
                        if(size>1000000||total>10000000)throw std::runtime_error("vector size/session limit exceeded");
                        std::vector<uint8_t> raw(size_t(size)*2);transfer(fd,raw.data(),raw.size(),false);
                        std::vector<uint16_t> values;values.reserve(size);
                        for(size_t j=0;j<raw.size();j+=2)values.push_back(uint16_t(uint16_t(raw[j])|(uint16_t(raw[j+1])<<8)));
                        write_number(fd,sum_squares(values),2);
                    }
                }catch(const std::exception& e){logger.write(false,context+" "+e.what());}
            }
        }catch(const std::exception& e){logger.write(true,e.what());throw;}
    }catch(const std::exception& e){std::cerr<<"Error: "<<e.what()<<'\n';return 1;}
    return 0;
}
