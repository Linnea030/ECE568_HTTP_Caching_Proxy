#include <algorithm>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "proxy.h"
#include <pthread.h>
#include <thread>
#include "socket_info.h"

#define MAX_LEN 65536
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
//std::ofstream logFile("/var/log/erss/proxy.log")

void Proxy::init_Proxy() {
    Csbuild cs;
    //test!!!
    std::cout<<"in init_proxy\n";
    //build as server
    fd_accept = cs.init_server(port);
    //test!!!
    std::cout<<"after build server\n";
    if(fd_accept == -1) {
        //pthread_mutex_lock(&lock);
        //logFile << "(no-id): ERROR in creating socket for proxy to accept\n";
        //pthread_mutex_unlock(&lock);
    }
    int thread_id = 0;

    //need to add multi-thread!!!
    while(1) {
        std::string ip;
        int fd_client = cs.accept_as_server(fd_accept, ip);
        if(fd_client == -1){
            //pthread_mutex_lock(&lock);
            //logFile<< "ERROR in connecting client\n";
            //pthread_mutex_unlock(&lock);
            std::cout<<"accept client failed\n";
        	continue;
		}
        
        //test!!!
        std::cout<<"accept client in init_proxy\n";


        pthread_t new_thread;
        pthread_mutex_lock(&lock);
        int fd_server = -1;
        SocketInfo * thread_info = new SocketInfo(fd_accept, fd_client, fd_server, thread_id, ip);
        thread_id++;
        pthread_mutex_unlock(&lock);
        pthread_create(&new_thread, NULL, process, thread_info);
        //std::thread t(&Proxy::process, this, thread_info);
        //t.join();
    }
}

void * Proxy::process(void * thread1) {
        SocketInfo *thread_info = (SocketInfo *)thread1;
        Csbuild cs;
        //receive request from remote client
        char request_info[MAX_LEN] = {0};
        int flag_size = recv(thread_info->fd_client, request_info, sizeof(request_info), 0);
        //判断recv的size =0 就结束这个handlereq， 小于0就400
        if(flag_size == 0) return NULL;

        //test!!!
        std::cout << "received request is:\n" << request_info << std ::endl;

        std::string request_info_s(request_info);

        //get an object of request package
        PackRequest req_pack(request_info_s);
        //test!!!
        //req_pack.print_request();

        //connect to client as server
        const char * h = req_pack.hostname.c_str();
        const char * p = req_pack.port.c_str();
        //test!!!
        std::cout<<"\n";
        std::cout << h << ":" << p << std::endl;

        thread_info->fd_server = cs.init_client(h, p);
        if(thread_info->fd_server == -1){
            //pthread_mutex_lock(&lock);
            //logFile<< "ERROR in connecting client\n";
            //pthread_mutex_unlock(&lock);
            std::cout<<"connect server failed\n";
        	return NULL;
		}

        //test!!!
        thread_info->info_print();

        //update_cs();
        if(req_pack.method == "GET"){
            //get function
        }
        else if(req_pack.method == "POST") {
            //post function
            
        }
        else if(req_pack.method == "CONNECT"){
            //test!!!
            std::cout<<"here method is connect\n";

            //connect function
            connect_function(thread_info->fd_client, 
                            thread_info->fd_server, thread_info->id);
            //processCONNECT();
            std::cout<<"after connect function\n";
            //pthread_mutex_lock(&lock);
            //print log thread_id << ": Tunnel closed"<<endl;
            //pthread_mutex_unlock(&lock);
        }
        else{
            //400 function
        }

        delete thread_info;
        return NULL;
}

void Proxy::connect_function(int fd_client, int fd_server, int id){
    //cs.cs_print();
    //proxy_print();
    //test!!!
    std::cout<<"in connect function\n";
    //print log ID: Requesting "REQUEST" from SERVER
    std::string con_info = "HTTP/1.1 200 OK\r\n\r\n";
    int con_info_len = con_info.size();
    const char * con_info_c = con_info.c_str();
    //send 200OK to remote client
    int flag_size_s;
    flag_size_s = send(fd_client, con_info_c, con_info_len, MSG_NOSIGNAL);
    std::cout<<"send 200OK to client with flag: "<<flag_size_s<<"\n";

    if (flag_size_s < 0) {
        //print Log(id, ": ERROR respond connect fails");
        perror("recv error: ");
        return;
    }

    // char buffer[65536] = {0};
    // int flag_recv = recv(fd_client, buffer, sizeof(buffer),0);
    // std::cout << "flag_recv "<<flag_recv<<std::endl;
    // std::cout << "buffer is: "<<buffer<<std::endl;

    //print log ID: Responding "RESPONSE"
    //while(1);
    
    // while(1){
    //     fd_set readfds;
    //     int max_fd = std::max(fd_client, fd_server);
    //     FD_ZERO(&readfds);
    //     FD_SET(fd_server, &readfds);
    //     FD_SET(fd_client, &readfds);
    //     select(max_fd + 1,&readfds, NULL, NULL, NULL);
    //     std::vector<int> fd_set_cs{fd_client, fd_server};

    //     int len_recv;
    //     int len_send;
    //     //int i = 0;
    //     for(int i = 0 ; i < 2 ; i ++){
    //         //vector<char> msg2(65536,0);
    //         char msg2[65536] = {0};
    //         if(FD_ISSET(fd_set_cs[i], &readfds)){
    //             //len_recv = Proxy::recv_message(fd_set_cs[i], &msg2, false);
    //             len_recv = recv(fd_set_cs[i], msg2, sizeof(msg2), MSG_NOSIGNAL);
    //             std::cout<<"recv info from client with flag: "<<len_recv<<"\n";
    //             if(len_recv <= 0){
    //                 perror("recv error: ");
    //                 return;
    //             }
    //             //string temp(msg2.begin(), msg2.end());
    //             // Check the len_send and len_recv
    //             //len_send = send(fd_set_cs[1-i], temp.c_str(), len_recv, 0);
    //             len_send = send(fd_set_cs[1-i], msg2, len_recv, MSG_NOSIGNAL);
    //             std::cout<<"send info to server with flag: "<<len_recv<<"\n";
    //             // If all of the message is done.
    //             if(len_send <= 0){
    //                 perror("send error: ");
    //                 return;
    //             }
    //         }
    //         //i++;
    //     } 
    // }
    

    
    //select and send message
    while(1) {
        fd_set readfds;
        int max_fd = std::max(fd_client, fd_server);
        FD_ZERO(&readfds);
        FD_SET(fd_server, &readfds);
        FD_SET(fd_client, &readfds);
        std::cout<<"before select\n";
        select(max_fd + 1, &readfds, NULL, NULL, NULL);
        //test!!!
        std::cout<<"in while and receive sth\n";

        //for(int i = 0; i < 2; i++) {
            char buf[MAX_LEN] = {0};
            int flag_r;
            int flag_s;
            if(FD_ISSET(fd_client, &readfds)) {
                //test!!!
                std::cout<<"recv info from client with flag: ";
                //get info from remote client
                flag_r = recv(fd_client, &buf, sizeof(buf), MSG_NOSIGNAL);
                std::cout<<buf;
                //test!!!
                std::cout<<flag_r<<"\n";
                if(flag_r <= 0) {
                    perror("recv error");
                    return;
                }

                //test!!!
                std::cout<<"send info to server with flag: ";
                //send to remote server
                flag_s = send(fd_server, buf, flag_r, MSG_NOSIGNAL);
                //test!!!
                std::cout<<flag_s<<"\n";
                if(flag_s <= 0) {
                    perror("send error");
                    return;
                }
            }
            if(FD_ISSET(fd_server, &readfds)) {
                //test!!!
                std::cout<<"recv info from server with flag: ";
                //get info from remote server
                flag_r = recv(fd_server, &buf, sizeof(buf), MSG_NOSIGNAL);
                //test!!!
                std::cout<<flag_r<<"\n";
                if(flag_r <= 0) {
                    perror("recv error");
                    return;
                }

                //test!!!
                std::cout<<"send info to client with flag: ";
                //send to remote client
                flag_s = send(fd_client, buf, flag_r, MSG_NOSIGNAL);
                //test!!!
                std::cout<<flag_s<<"\n";
                if(flag_s <= 0) {
                    perror("send error");
                    return;
                }
            }
        //}
    }
}

///////////////////////////////
/*void Proxy::connect_function(){
    cs.cs_print();
    proxy_print();
    std::string msg = "HTTP/1.1 200 OK\r\n\r\n";
    send(fd_accept, msg.c_str() , msg.size(), 0);


    fd_set fds;
    int nfds = std::max(fd_accept, fd_client) ;

    bool flag = true;
    while(flag){
        FD_ZERO(&fds);
        FD_SET(fd_client, &fds);
        FD_SET(fd_accept, &fds);
        select(nfds + 1,&fds, NULL, NULL, NULL);
        std::vector<int> fd_set_cs{fd_accept, fd_client};

        int len_recv;
        int len_send;
        //int i = 0;
        for(int i = 0 ; i < 2 ; i ++){
            //vector<char> msg2(65536,0);
            char msg2[65536] = {0};
            if(FD_ISSET(fd_set_cs[i], &fds)){
                //len_recv = Proxy::recv_message(fd_set_cs[i], &msg2, false);
                len_recv = recv(fd_set_cs[i], msg2, sizeof(msg2), MSG_WAITALL);
                std::cout<<"recv info from client with flag: "<<len_recv<<"\n";
                if(len_recv <= 0){
                    return;
                }
                //string temp(msg2.begin(), msg2.end());
                // Check the len_send and len_recv
                //len_send = send(fd_set_cs[1-i], temp.c_str(), len_recv, 0);
                len_send = send(fd_set_cs[1-i], msg2, len_recv, MSG_WAITALL);
                std::cout<<"send info to server with flag: "<<len_recv<<"\n";
                // If all of the message is done.
                if(len_send <=0){
                    return;
                }
            }
            //i++;
        } 
    }
}
*/

// void Proxy::update_cs() {
//     fd_server = cs.fd_server;//used to accept remote client
//     fd_accept = cs.fd_accept;//used to send and recv from remote client
//     fd_client = cs.fd_client;//used to connect remote server
//     ip_client = cs.ip_client;//ip for remote client used to print in log file
// }

/*void * Proxy::processCONNECT(){
    // step2: send an http response of "200 ok" back to the browser
    int byte_count = send(fd_accept, "HTTP/1.1 200 OK\r\n\r\n", 19, 0);
    if (byte_count <= 0) {
        return NULL;
    }
    // step3: use IO multiplexing (select())
    struct timeval tv;
    fd_set readfds;
    int maxfd = 0;
    int rv = 0;
    int client_socket = fd_accept;
    int server_socket = fd_client;

    char buf[MAX_LEN] = {0};
    while (true) {
        int byte_count;
        tv.tv_sec = 2; 
        tv.tv_usec = 0; 
        // clear sets and add our descriptors
        FD_ZERO(&readfds);
        FD_SET(client_socket, &readfds); // socket one
        FD_SET(server_socket, &readfds); // socket two
        maxfd = client_socket > server_socket ? client_socket : server_socket;
        rv = select(maxfd + 1, &readfds, NULL, NULL, &tv);
        if (rv == -1) {
            perror("select"); // error occurred in select()
        } else if (rv == 0) {
            printf("Timeout occurred! No data after 10.5 seconds.\n");
        } else {
            // one or both of the descriptors have data
            if (FD_ISSET(client_socket, &readfds)) {
                byte_count = recv(client_socket, buf, sizeof(buf), 0);
                std::cout << "connect recv1" << std::endl;
                if(byte_count <= 0){
                    perror("recv error");
                    return NULL;
                }
                byte_count = send(server_socket, buf, byte_count, 0);
                std::cout << "connect send1" << std::endl;
                if (byte_count <= 0){
                    perror("send error");
                    return NULL;
                }

            }
            if (FD_ISSET(server_socket, &readfds)) {
                byte_count = recv(server_socket, buf, sizeof(buf), 0);
                std::cout << "connect recv2" << std::endl;
                if(byte_count <= 0){
                    perror("recv error");
                    return NULL;
                }
                byte_count = send(client_socket, buf, byte_count, 0);
                std::cout << "connect recv2" << std::endl;
                if (byte_count <= 0){
                    perror("send error");
                    return NULL;
                }
            }
        }
    }
};
*/