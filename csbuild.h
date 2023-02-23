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
    
    int init_server(char * port) {
        int status;
        int socket_fd;
        struct addrinfo host_info;
        struct addrinfo *host_info_list;

        memset(&host_info, 0, sizeof(host_info));

        host_info.ai_family   = AF_UNSPEC;
        host_info.ai_socktype = SOCK_STREAM;
        host_info.ai_flags    = AI_PASSIVE;

        //trans int to const char*
        std::stringstream ss;
        ss<<port_num;
        std::string port_num_s = ss.str();
        char * port_num_c = new char[port_num_s.length() + 1];
        std::strcpy(port_num_c, port_num_s.c_str());

        status = getaddrinfo(NULL, port_num_c, &host_info, &host_info_list);
        if (status != 0) {
            std::cerr << "Error: cannot get address info for host\n";
            exit(EXIT_FAILURE);
            delete[] port_num_c;  //free memory of path
        } //if

        delete[] port_num_c;  //free memory of path

        socket_fd = socket(host_info_list->ai_family, 
                    host_info_list->ai_socktype, 
                    host_info_list->ai_protocol);
        if (socket_fd == -1) {
            std::cerr << "Error: cannot create socket\n";
            exit(EXIT_FAILURE);
        } //if
        fd_master = socket_fd;

        int yes = 1;
        status = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
        status = bind(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
        if (status == -1) {
            std::cerr << "Error: cannot bind socket\n";
            exit(EXIT_FAILURE);
        } //if

        status = listen(socket_fd, 100);
        if (status == -1) {
            std::cerr << "Error: cannot listen on socket\n"; 
            exit(EXIT_FAILURE);
        } //if
    }

};