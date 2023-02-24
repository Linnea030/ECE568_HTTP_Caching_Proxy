#include <pthread.h>
#include "csbuild.h"
#include "package.h"

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

    void init_Proxy();
    void connect_function();
    void update_cs();
    void proxy_print(){
        std::cout<<"info of proxy:\n";
        std::cout<<"fd_server: "<<fd_server<<" fd_accept: "<<fd_accept<<" fd_client: "<<fd_client<<" ip_client: "<<ip_client<<"\n";
        std::cout<<"\n";
    }

    ~Proxy() {
        close(fd_server);
        close(fd_accept);
        close(fd_client);
    }
};