#include "proxy.h"
#include "csbuild.h"
#include "package.h"

#define MAX_LEN 65536
pthread_mutex_t lock;
//std::ofstream logFile("/var/log/erss/proxy.log");

void Proxy::init_Proxy() {
    //build as server
    fd_server = cs.init_server(port);
    if(fd_server == -1) {
        pthread_mutex_lock(&lock);
        //logFile << "(no-id): ERROR in creating socket for proxy to accept\n";
        pthread_mutex_unlock(&lock);
    }

    //need to add multi-thread!!!
    while(1) {
        std::string ip_client;
        fd_accept = cs.accept_as_server(ip_client);
        if(fd_accept == -1){
            pthread_mutex_lock(&lock);
            //logFile<< "ERROR in connecting client\n";
            pthread_mutex_unlock(&lock);
        	continue;
		}

        //receive request from remote client
        char request_info[MAX_LEN] = {0};
        int flag_size = recv(fd_accept, &request_info, sizeof(request_info), MSG_WAITALL);
        //判断recv的size =0 就结束这个handlereq， 小于0就400    
        std::string request_info_s(request_info, flag_size);

        //get an object of request package
        PackRequest req_pack(request_info_s);
        //test!!!
        req_pack.print_request();
        //connect to client as server
        const char * hostname = req_pack.hostname.c_str();
        const char * port = req_pack.port.c_str();
        cs.init_client(hostname, port);
    }

    
}

/*
void Proxy::handleConnect(int client_fd, int server_fd, int thread_id){
    string msg = "HTTP/1.1 200 OK\r\n\r\n";
    send(client_fd, msg.c_str() , msg.size(), MSG_NOSIGNAL);

    pthread_mutex_lock(&mutex);
    logFile << thread_id << ": Responding \"HTTP/1.1 200 OK\""<<endl;
    pthread_mutex_unlock(&mutex);

    fd_set fds;
    int nfds = max(client_fd, server_fd) ;

    bool flag = true;
    while(flag){
        FD_ZERO(&fds);
        FD_SET(server_fd, &fds);
        FD_SET(client_fd, &fds);
        select(nfds + 1,&fds, NULL, NULL, NULL);
        vector<int> fd_set_cs{client_fd, server_fd};

        int len_recv;
        int len_send;
        //int i = 0;
        for(int i = 0 ; i < 2 ; i ++){
            //vector<char> msg2(65536,0);
            char msg2[65536] = {0};
            if(FD_ISSET(fd_set_cs[i], &fds)){
                //len_recv = Proxy::recv_message(fd_set_cs[i], &msg2, false);
                len_recv = recv(fd_set_cs[i], msg2, sizeof(msg2), MSG_NOSIGNAL);
                if(len_recv <= 0){
                    return;
                }
                //string temp(msg2.begin(), msg2.end());
                // Check the len_send and len_recv
                //len_send = send(fd_set_cs[1-i], temp.c_str(), len_recv, 0);
                len_send = send(fd_set_cs[1-i], msg2, len_recv, MSG_NOSIGNAL);
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