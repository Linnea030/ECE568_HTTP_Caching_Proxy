#ifndef __CS_BUILD__
#define __CS_BUILD__
#include <iostream>
#include <vector>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

class Csbuild {
    public:

    // std::string ip_client;//ip for remote client used to print in log file

    //Csbuild() : fd_server(-1), fd_accept(-1), fd_client(-1) {}

    //initialize as server
    int init_server(const char * port);
    //initialize as client and connect remote server
    int init_client(const char * hostname, const char * port);
    //accept remote client as a server
    int accept_as_server(int fd_accept, std::string & ip);
    /*void cs_print() {
        std::cout<<"info of cs:\n";
        std::cout<<"fd_server: "<<fd_server<<" fd_accept: "<<fd_accept<<" fd_client: "<<fd_client<<" ip_client： "<<ip_client<<"\n";
        std::cout<<"\n";
    }*/
    ~Csbuild() {}
};
#endif