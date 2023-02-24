#include "csbuild.h"
//remote server means S in C-P-S 
//remote client means C in C-P-S
//remote client port:12345
//remote server port:443

//build as a server and listen to remote client with port input
int Csbuild::init_server(const char * port) {//this port is set in main.cpp
    int status;
    int socket_fd;
    struct addrinfo host_info;
    struct addrinfo *host_info_list;
    const char* hostname = NULL;

    memset(&host_info, 0, sizeof(host_info));

    host_info.ai_family   = AF_UNSPEC;
    host_info.ai_socktype = SOCK_STREAM;
    host_info.ai_flags    = AI_PASSIVE;

    status = getaddrinfo(hostname, port, &host_info, &host_info_list);
    if (status != 0) {
        std::cerr << "Error: cannot get address info for host\n";
        return -1;
    } //if

    socket_fd = socket(host_info_list->ai_family, 
                host_info_list->ai_socktype, 
                host_info_list->ai_protocol);
    if (socket_fd == -1) {
        std::cerr << "Error: cannot create socket\n";
        return -1;
    } //if

    int yes = 1;
    status = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    status = bind(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
    if (status == -1) {
        std::cerr << "Error: cannot bind socket\n";
        return -1;
    } //if

    status = listen(socket_fd, 100);
    if (status == -1) {
        std::cerr << "Error: cannot listen on socket\n"; 
        return -1;
    } //if
    
    //fd_accept = socket_fd;
    freeaddrinfo(host_info_list);
    return socket_fd;
}

//initialize as a client and connect to remote server
int Csbuild::init_client(const char * hostname, const char * port) {
    int status;
    int socket_fd;
    struct addrinfo host_info;
    struct addrinfo *host_info_list;

    memset(&host_info, 0, sizeof(host_info));
    host_info.ai_family   = AF_UNSPEC;
    host_info.ai_socktype = SOCK_STREAM;

    status = getaddrinfo(hostname, port, &host_info, &host_info_list);
    if (status != 0) {
        std::cerr << "Error: cannot get address info for host\n";
        std::cerr << "  (" << hostname<< "," << port<< ")'n";
        return -1;
    } //if

    socket_fd = socket(host_info_list->ai_family, 
                    host_info_list->ai_socktype, 
                    host_info_list->ai_protocol);

    if (socket_fd == -1) {
        std::cerr << "Error: cannot create socket\n";
        std::cerr << "  (" << hostname<< "," << port<< ")\n";
        return -1;
    } //if

    //connect to remote server
    status = connect(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
    if (status == -1) {
        std::cerr << "Error: cannot connect to socket\n";
        std::cerr << "  (" << hostname<< "," << port<< ")\n";
        return -1;
    } //if

    //fd_server = socket_fd;
    freeaddrinfo(host_info_list);
    return socket_fd;
}

//accept remote client as a server
int Csbuild::accept_as_server(int fd_accept, std::string & ip) {
    struct sockaddr_storage socket_addr;
    socklen_t socket_addr_len = sizeof(socket_addr);
    int socket_fd;
    socket_fd = accept(fd_accept, (struct sockaddr *)&socket_addr, &socket_addr_len);
    struct sockaddr_in * addr = (struct sockaddr_in *)&socket_addr;

    if (socket_fd == -1) {
        std::cerr << "Error: cannot accept connection on socket\n";
        return -1;
    } //if

    //fd_client = socket_fd;
    //used addr to get ip
    
    ip = inet_ntoa(addr->sin_addr);
    //ip_client = ip;
    return socket_fd;
}
