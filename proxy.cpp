#include <algorithm>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "proxy.h"

#define MAX_LEN 65536
pthread_mutex_t lock;
//std::ofstream logFile("/var/log/erss/proxy.log");

void Proxy::init_Proxy() {
    //test!!!
    std::cout<<"in init_proxy\n";
    //build as server
    fd_server = cs.init_server(port);
    //test!!!
    std::cout<<"after build server\n";
    if(fd_server == -1) {
        //pthread_mutex_lock(&lock);
        //logFile << "(no-id): ERROR in creating socket for proxy to accept\n";
        //pthread_mutex_unlock(&lock);
    }

    //need to add multi-thread!!!
    while(1) {
        std::string ip_client;
        fd_accept = cs.accept_as_server(ip_client);
        //test!!!
        std::cout<<"accept client in init_proxy\n";

        if(fd_accept == -1){
            pthread_mutex_lock(&lock);
            //logFile<< "ERROR in connecting client\n";
            pthread_mutex_unlock(&lock);
            std::cout<<"accept client failed\n";
        	continue;
		}

        //receive request from remote client
        char request_info[MAX_LEN] = {0};
        int flag_size = recv(fd_accept, &request_info, sizeof(request_info), MSG_WAITALL);
        //判断recv的size =0 就结束这个handlereq， 小于0就400    

        //test!!!
        std::cout << "received request is:\n" << request_info << std ::endl;
        std::string request_info_s(request_info, flag_size);

        //get an object of request package
        PackRequest req_pack(request_info_s);
        //test!!!
        req_pack.print_request();

        //connect to client as server
        const char * hostname = req_pack.hostname.c_str();
        const char * port = req_pack.port.c_str();
        //std::cout << hostname << ":" << port << std::endl;
        cs.init_client(hostname, port);

        update_cs();
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
            connect_function();
            std::cout<<"after connect function\n";
            //pthread_mutex_lock(&lock);
            //print log thread_id << ": Tunnel closed"<<endl;
            //pthread_mutex_unlock(&lock);
        }
        else{
            //400 function
        }
    }
}

void Proxy::connect_function(){
    cs.cs_print();
    proxy_print();
    //test!!!
    std::cout<<"in connect function\n";
    //print log ID: Requesting "REQUEST" from SERVER
    std::string con_info = "HTTP/1.1 200 OK\r\n\r\n";
    int con_info_len = con_info.size();
    const char * con_info_c = con_info.c_str();

    //send 200OK to remote client
    int flag_size = send(fd_accept, con_info_c, con_info_len, 0);
    std::cout<<"send 200OK to client with flag: "<<flag_size<<"\n";

    if (flag_size < 0) {
        //print Log(id, ": ERROR respond connect fails");
        return;
    }
    //print log ID: Responding "RESPONSE"
    //while(1);
    fd_set readfds;
    int max_fd = std::max(fd_client, fd_accept);

    //select and send message
    while(1) {
        FD_ZERO(&readfds);
        FD_SET(fd_accept, &readfds);
        FD_SET(fd_client, &readfds);
        std::cout<<"before select\n";
        select(max_fd + 1, &readfds, NULL, NULL, NULL);
        //test!!!
        std::cout<<"in while and receive sth\n";

        for(int i = 0; i < 2; i++) {
            char buf[65536] = {0};
            int flag_r;
            int flag_s;
            if(FD_ISSET(fd_accept, &readfds)) {
                //test!!!
                std::cout<<"recv info from client with flag: ";
                //get info from remote client
                flag_r = recv(fd_accept, &buf, sizeof(buf), MSG_WAITALL);
                std::cout<<buf;
                //test!!!
                std::cout<<flag_r<<"\n";
                if(flag_r <= 0) return;

                //test!!!
                std::cout<<"send info to server with flag: ";
                //send to remote server
                flag_s = send(fd_client, buf, flag_r, 0);
                //test!!!
                std::cout<<flag_s<<"\n";
                //if(flag_s <= 0) return;
            }
            if(FD_ISSET(fd_client, &readfds)) {
                //test!!!
                std::cout<<"recv info from server with flag: ";
                //get info from remote server
                flag_r = recv(fd_client, &buf, sizeof(buf), MSG_WAITALL);
                //test!!!
                std::cout<<flag_r<<"\n";
                //if(flag_r <= 0) return;

                //test!!!
                std::cout<<"send info to client with flag: ";
                //send to remote client
                flag_s = send(fd_accept, buf, flag_r, 0);
                //test!!!
                std::cout<<flag_s<<"\n";
                //if(flag_s <= 0) return;
            }
        }
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

void Proxy::update_cs() {
    fd_server = cs.fd_server;//used to accept remote client
    fd_accept = cs.fd_accept;//used to send and recv from remote client
    fd_client = cs.fd_client;//used to connect remote server
    ip_client = cs.ip_client;//ip for remote client used to print in log file
}