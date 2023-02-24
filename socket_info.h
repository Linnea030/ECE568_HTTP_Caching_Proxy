#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

class SocketInfo {
    public:
    int fd_accept;//used to accept remote client
    int fd_client;//used to send and recv from remote client
    int fd_server;//used to connect remote server
    int id;
    std::string ip_client;//ip for remote client used to print in log file
    void info_print(){
        std::cout<<"info of thread "<<id<<":\n";
        std::cout<<"fd_accept: "<<fd_accept<<"fd_server: "<<fd_server<<" fd_client: "<<fd_client<<" ip_client: "<<ip_client<<"\n";
        std::cout<<"\n";
    }

    SocketInfo(int fd_accept, int fd_client, int fd_server, int id, std::string ip_client) :
             fd_accept(fd_accept), fd_client(fd_client), fd_server(fd_server), id(id), ip_client(ip_client){}
    ~SocketInfo() {
        close(fd_server);
        close(fd_client);
    }
};