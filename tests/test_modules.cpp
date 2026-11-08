#ifdef NDEBUG
#error Tests require assertions: compile without NDEBUG
#endif
#include "core.h"
#include "net.h"
#include "logger.h"
#include <regex>
#include <cassert>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <fcntl.h>
#include <cstdlib>
#include <type_traits>
using namespace course;
static_assert(!std::is_copy_constructible<Socket>::value, "Socket must not be copyable");
class TempFile {
public:
    std::string path;
    explicit TempFile(const std::string& text) {
        char name[]="/tmp/coursework-test-XXXXXX";
        int fd=mkstemp(name);
        if(fd<0)throw std::runtime_error("mkstemp failed");
        close(fd); path=name;
        std::ofstream out(path,std::ios::binary);
        if(!out.write(text.data(),text.size()))throw std::runtime_error("fixture write failed");
    }
    ~TempFile(){std::remove(path.c_str());}
    TempFile(const TempFile&)=delete;
    TempFile& operator=(const TempFile&)=delete;
};
int main() {
    std::map<std::string,std::string> users{{"user","P@ssW0rd"}};
    std::cout << "T01 RUN" << std::endl;
    { assert(sum_squares({}) == 0); }
    std::cout << "T01 PASS" << std::endl;
    std::cout << "T02 RUN" << std::endl;
    { assert(sum_squares({1,2,3,4}) == 30); }
    std::cout << "T02 PASS" << std::endl;
    std::cout << "T03 RUN" << std::endl;
    { assert(sum_squares({255}) == 65025); }
    std::cout << "T03 PASS" << std::endl;
    std::cout << "T04 RUN" << std::endl;
    { assert(sum_squares({256}) == 65535); }
    std::cout << "T04 PASS" << std::endl;
    std::cout << "T05 RUN" << std::endl;
    { assert(sum_squares({255,22,5}) == 65534); }
    std::cout << "T05 PASS" << std::endl;
    std::cout << "T06 RUN" << std::endl;
    { assert(sum_squares({255,22,5,1}) == 65535); }
    std::cout << "T06 PASS" << std::endl;
    std::cout << "T07 RUN" << std::endl;
    { assert(sha1("") == "DA39A3EE5E6B4B0D3255BFEF95601890AFD80709"); }
    std::cout << "T07 PASS" << std::endl;
    std::cout << "T08 RUN" << std::endl;
    { assert(sha1("abc") == "A9993E364706816ABA3E25717850C26C9CD0D89D"); }
    std::cout << "T08 PASS" << std::endl;
    std::cout << "T09 RUN" << std::endl;
    { assert(sha1(std::string(56, 'a')) == "C2DB330F6083854C99D4B5BFB6E8F29F201BE699"); }
    std::cout << "T09 PASS" << std::endl;
    std::cout << "T10 RUN" << std::endl;
    { assert(sha1(std::string(1000000, 'a')) == "34AA973CD4C4DAA4F61EEB2BDBAD27316534016F"); }
    std::cout << "T10 PASS" << std::endl;
    std::cout << "T11 RUN" << std::endl;
    { assert(authenticate("user000000000000000109B121B4F0C23BAB9AA51EB4386CA859E9C49D41", users) == true); }
    std::cout << "T11 PASS" << std::endl;
    std::cout << "T12 RUN" << std::endl;
    { assert(authenticate("user000000000000000150E0BCCA6D5BC90D6DDD9205D27138C72B04674E", users) == false); }
    std::cout << "T12 PASS" << std::endl;
    std::cout << "T13 RUN" << std::endl;
    { assert(authenticate("other000000000000000109B121B4F0C23BAB9AA51EB4386CA859E9C49D41", users) == false); }
    std::cout << "T13 PASS" << std::endl;
    std::cout << "T14 RUN" << std::endl;
    { assert(authenticate("userG00000000000000109B121B4F0C23BAB9AA51EB4386CA859E9C49D41", users) == false); }
    std::cout << "T14 PASS" << std::endl;
    std::cout << "T15 RUN" << std::endl;
    { assert(authenticate("user0000000000000001G9B121B4F0C23BAB9AA51EB4386CA859E9C49D41", users) == false); }
    std::cout << "T15 PASS" << std::endl;
    std::cout << "T16 RUN" << std::endl;
    { assert(authenticate("user000000000000000109B121B4F0C23BAB9AA51EB4386CA859E9C49D4", users) == false); }
    std::cout << "T16 PASS" << std::endl;
    std::cout << "T17 RUN" << std::endl;
    { TempFile f("user:P@ssW0rd\n"); auto db=load_users(f.path); assert(db.size()==1); assert(db.at("user")=="P@ssW0rd"); }
    std::cout << "T17 PASS" << std::endl;
    std::cout << "T18 RUN" << std::endl;
    { TempFile f("user:a\nsecond:b\n"); auto db=load_users(f.path); assert(db.size()==2); assert(db.at("user")=="a"); assert(db.at("second")=="b"); }
    std::cout << "T18 PASS" << std::endl;
    std::cout << "T19 RUN" << std::endl;
    { TempFile f(""); bool thrown=false; try {load_users(f.path);} catch(const std::runtime_error&) {thrown=true;} assert(thrown); }
    std::cout << "T19 PASS" << std::endl;
    std::cout << "T20 RUN" << std::endl;
    { TempFile f("user\n"); bool thrown=false; try {load_users(f.path);} catch(const std::runtime_error&) {thrown=true;} assert(thrown); }
    std::cout << "T20 PASS" << std::endl;
    std::cout << "T21 RUN" << std::endl;
    { TempFile f("user:a\nuser:b\n"); bool thrown=false; try {load_users(f.path);} catch(const std::runtime_error&) {thrown=true;} assert(thrown); }
    std::cout << "T21 PASS" << std::endl;
    std::cout << "T22 RUN" << std::endl;
    { TempFile f(""); assert(std::remove(f.path.c_str())==0); bool thrown=false; try {load_users(f.path);} catch(const std::runtime_error&) {thrown=true;} assert(thrown); }
    std::cout << "T22 PASS" << std::endl;
    std::cout << "T23 RUN" << std::endl;
    { assert(parse_number("33333",65535)==33333); }
    std::cout << "T23 PASS" << std::endl;
    std::cout << "T24 RUN" << std::endl;
    { bool thrown=false; try {parse_number("1x",65535);} catch(const std::runtime_error&) {thrown=true;} assert(thrown); }
    std::cout << "T24 PASS" << std::endl;
    std::cout << "T25 RUN" << std::endl;
    { bool thrown=false; try {parse_number("65536",65535);} catch(const std::runtime_error&) {thrown=true;} assert(thrown); }
    std::cout << "T25 PASS" << std::endl;
    std::cout << "T26 RUN" << std::endl;
    { bool thrown=false; try {Socket s(-1);} catch(const std::runtime_error&) {thrown=true;} assert(thrown); }
    std::cout << "T26 PASS" << std::endl;
    std::cout << "T27 RUN" << std::endl;
    { TempFile f("a"); int fd=::open(f.path.c_str(),O_RDONLY); assert(fd>=0); {Socket s(fd); assert(s.get()==fd);} errno=0; assert(fcntl(fd,F_GETFD)==-1); assert(errno==EBADF); }
    std::cout << "T27 PASS" << std::endl;
    std::cout << "T28 RUN" << std::endl;
    { TempFile f(""); {Logger log(f.path); log.write(true,"failure");} std::ifstream in(f.path); std::string line; std::getline(in,line); assert(std::regex_match(line,std::regex("[0-9]{4}-[0-9]{2}-[0-9]{2} [0-9]{2}:[0-9]{2}:[0-9]{2} CRITICAL failure"))); }
    std::cout << "T28 PASS" << std::endl;
    std::cout << "T29 RUN" << std::endl;
    { TempFile f("old\n"); {Logger log(f.path); log.write(false,"peer=test");} std::ifstream in(f.path); std::string a,b; std::getline(in,a); std::getline(in,b); assert(a=="old"); assert(b.find(" NONCRITICAL peer=test")!=std::string::npos); }
    std::cout << "T29 PASS" << std::endl;
    std::cout << "T30 RUN" << std::endl;
    { TempFile f(""); bool thrown=false; try {Logger log(f.path+"/server.log");} catch(const std::runtime_error&) {thrown=true;} assert(thrown); }
    std::cout << "T30 PASS" << std::endl;
    std::cout << "TOTAL 30 PASSED 30 FAILED 0" << std::endl;
    return 0;
}
