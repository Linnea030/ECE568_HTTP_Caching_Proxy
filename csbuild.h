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
    int fd_server;//used to accept remote client
    int fd_accept;//used to send and recv from remote client
    int fd_client;//used to connect remote server

    std::string ip_client;//ip for remote client used to print in log file

    Csbuild() : fd_server(-1), fd_accept(-1), fd_client(-1) {}

    //initialize as server
    int init_server(const char * port) {}
    //initialize as client and connect remote server
    int init_client(const char * hostname, const char * port) {}
    //accept remote client as a server
    int accept_as_server(std::string & ip) {}
    ~Csbuild() {}
};