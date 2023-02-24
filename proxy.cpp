#include <algorithm>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <proxy.h>

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
        //test!!!
        std::cout << "received client request is:\n" << request_info << std ::endl;
        std::string request_info_s(request_info, flag_size);

        //get an object of request package
        PackRequest req_pack(request_info_s);
        //test!!!
        req_pack.print_request();
        //connect to client as server
        const char * hostname = req_pack.hostname.c_str();
        const char * port = req_pack.port.c_str();
        cs.init_client(hostname, port);

        if(req_pack.method == "GET"){
            //get function
        }
        else if(req_pack.method == "POST") {
            //post function
            
        }
        else if(req_pack.method == "CONNECT"){
            //connect function
            connect_function();
            pthread_mutex_lock(&lock);
            //print log thread_id << ": Tunnel closed"<<endl;
            pthread_mutex_unlock(&lock);
        }
        else{
            //400 function
        }
    }
}

void Proxy::connect_function(){
    //print log ID: Requesting "REQUEST" from SERVER
    std::string con_info = "HTTP/1.1 200 OK\r\n\r\n";
    int con_info_len = con_info.size();
    //send 200OK to remote client
    int flag_size = send(fd_accept, con_info.c_str() , con_info_len, MSG_NOSIGNAL);
    if (flag_size < 0) {
        //print Log(id, ": ERROR respond connect fails");
        return;
    }
    //print log ID: Responding "RESPONSE"

    //select and send message
    while(1) {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(fd_accept, &readfds);
        FD_SET(fd_client, &readfds);
        int max_fd = std::max(fd_client, fd_accept);
        select(max_fd + 1, &readfds, NULL, NULL, NULL);
        
        for(int i = 0; i < 2; i++) {
            char buf[65536] = {0};
            int flag_r;
            int flag_s;
            if(FD_ISSET(fd_accept, &readfds)) {
                //get info from remote client
                flag_r = recv(fd_accept, &buf, sizeof(buf), MSG_WAITALL);
                if(flag_r <= 0) return;
                //send to remote server
                flag_s = send(fd_client, buf, flag_r, 0);
                if(flag_s <= 0) return;
            }
            else if(FD_ISSET(fd_client, &readfds)) {
                //get info from remote server
                flag_r = recv(fd_client, &buf, sizeof(buf), MSG_WAITALL);
                if(flag_r <= 0) return;
                //send to remote client
                flag_s = send(fd_accept, buf, flag_r, 0);
                if(flag_s <= 0) return;
            }
        }
    }
}