#include <pthread.h>
#include "csbuild.h"
#include "package.h"

class Proxy {
    public:
    const char * port;

    // ////////////////not necessary
    int fd_accept;//used to accept remote client
    // int fd_client;//used to send and recv from remote client
    // int fd_server;//used to connect remote server
    // std::string ip_client;//ip for remote client used to print in log file
    //Csbuild cs;

    //Proxy(const char * port) : port(port), fd_accept(-1), fd_client(-1), fd_server(-1), ip_client("") {}
    Proxy(const char * port) : port(port) {}
    void init_Proxy();
    static void connect_function(int fd_client, int fd_server, int id);
    //void update_cs();
    static void post_function(int fd_client, int fd_server, int id, PackRequest & request_info);
    static void function400(int fd_client, int id);
    static void * process(void * thread1);
    // void proxy_print(){
    //     std::cout<<"info of proxy:\n";
    //     std::cout<<"fd_server: "<<fd_server<<" fd_accept: "<<fd_accept<<" fd_client: "<<fd_client<<" ip_client: "<<ip_client<<"\n";
    //     std::cout<<"\n";
    // }

    //void * processCONNECT();

    ~Proxy() {
        // close(fd_server);
        // close(fd_accept);
        // close(fd_client);
    }
};