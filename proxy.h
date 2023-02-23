#include <pthread.h>

class Proxy {
    public:
    const char * port;
    Csbuild cs;
    ////////////////not necessary
    int fd_server;//used to accept remote client
    int fd_accept;//used to send and recv from remote client
    int fd_client;//used to connect remote server
    std::string ip_client;//ip for remote client used to print in log file

    Proxy(const char * port) : port(port) {}

    void init_Proxy() {}

    ~Proxy() {}
};