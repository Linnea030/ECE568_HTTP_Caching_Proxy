#include <algorithm>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "proxy.h"
#include <pthread.h>
#include <thread>
#include "socket_info.h"
#include "handler.h"


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
        std::cout<<"\n";
    }
}

void * Proxy::process(void * thread1) {
        SocketInfo *thread_info = (SocketInfo *)thread1;
        //test!!!
        std::cout<<"--------start a new process with id: "<<thread_info->id<<"------------\n";
        Csbuild cs;
        //receive request from remote client
        char request_info[MAX_LEN] = {0};
        int flag_size = recv(thread_info->fd_client, request_info, sizeof(request_info), 0);
        //判断recv的size =0 就结束这个handlereq， 小于0就400
        if(flag_size == 0) return NULL;

        //test!!!
        std::cout << "received request is!!!:\n" << request_info;

        std::string request_info_s(request_info);

        //get an object of request package
        PackRequest req_pack(request_info_s);
        //test!!!
        std::cout << "\nparse request info is!!!:\n";
        req_pack.print_request();

        //connect to client as server
        const char * h = req_pack.hostname.c_str();
        const char * p = req_pack.port.c_str();
        //test!!!
        //std::cout<<"\n";
        //std::cout << h << ":" << p << std::endl;

        thread_info->fd_server = cs.init_client(h, p);
        if(thread_info->fd_server == -1){
            //pthread_mutex_lock(&lock);
            //logFile<< "ERROR in connecting client\n";
            //pthread_mutex_unlock(&lock);
            std::cout<<"connect server failed in process\n";
        	return NULL;
		}

        //test!!!
        thread_info->info_print();

        //update_cs();
        if(req_pack.method == "GET"){
            std::cout<<"\n__________here method is GET__________\n";
            //get function
            Handler h;
            Cache cache;
            h.GETHandler(req_pack, thread_info->fd_client, thread_info->fd_server, cache, thread_info->id);
        }
        else if(req_pack.method == "POST") {
            //test!!!
            std::cout<<"\n__________here method is POST__________\n";
            //post function
            post_function(thread_info->fd_client, 
                            thread_info->fd_server, thread_info->id, req_pack);
            std::cout<<"after post function\n";
        }
        else if(req_pack.method == "CONNECT"){
            //test!!!
            std::cout<<"\n__________here method is CONNECT__________\n";

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
            std::cout<<"__________here method is 400__________\n";
            function400(thread_info->fd_client, thread_info->id);
        }
        delete thread_info;
        std::cout<<"--------new process with id: "<<thread_info->id<<"is over------------\n";
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
    //std::cout<<"send 200OK to client with flag: "<<flag_size_s<<"\n";

    if (flag_size_s < 0) {
        //print Log(id, ": ERROR respond connect fails");
        perror("recv error: ");
        return;
    }
    
    //select and send message
    while(1) {
        fd_set readfds;
        int max_fd = std::max(fd_client, fd_server);
        FD_ZERO(&readfds);
        FD_SET(fd_server, &readfds);
        FD_SET(fd_client, &readfds);
        //std::cout<<"before select\n";
        select(max_fd + 1, &readfds, NULL, NULL, NULL);
        //test!!!
        //std::cout<<"in while and receive sth\n";

        //for(int i = 0; i < 2; i++) {
            char buf[MAX_LEN] = {0};
            int flag_r;
            int flag_s;
            if(FD_ISSET(fd_client, &readfds)) {
                //test!!!
                //std::cout<<"recv info from client with flag: ";
                //get info from remote client
                flag_r = recv(fd_client, &buf, sizeof(buf), MSG_NOSIGNAL);
                //std::cout<<buf;
                //test!!!
                //std::cout<<flag_r<<"\n";
                if(flag_r <= 0) {
                    perror("recv error");
                    return;
                }

                //test!!!
                //std::cout<<"send info to server with flag: ";
                //send to remote server
                flag_s = send(fd_server, buf, flag_r, MSG_NOSIGNAL);
                //test!!!
                //std::cout<<flag_s<<"\n";
                if(flag_s <= 0) {
                    perror("send error");
                    return;
                }
            }
            if(FD_ISSET(fd_server, &readfds)) {
                //test!!!
                //std::cout<<"recv info from server with flag: ";
                //get info from remote server
                flag_r = recv(fd_server, &buf, sizeof(buf), MSG_NOSIGNAL);
                //test!!!
                //std::cout<<flag_r<<"\n";
                if(flag_r <= 0) {
                    perror("recv error");
                    return;
                }

                //test!!!
                //std::cout<<"send info to client with flag: ";
                //send to remote client
                flag_s = send(fd_client, buf, flag_r, MSG_NOSIGNAL);
                //test!!!
                //std::cout<<flag_s<<"\n";
                if(flag_s <= 0) {
                    perror("send error");
                    return;
                }
            }
        //}
    }
}

void Proxy::post_function(int fd_client, int fd_server, int id, PackRequest & request_info) {
    //test!!!
    std::cout<<"in post function\n";
    if (request_info.content_len_remain != -1) {
        int flag_size_s;
        const char * res_info_c = request_info.request.c_str();
        flag_size_s = send(fd_server, res_info_c, request_info.request.size(), MSG_NOSIGNAL);//0 in xy
        std::cout<<"send request to server with flag: "<<flag_size_s<<"\n";

        char response[65536] = {0};
        int response_len = recv(fd_server,response,sizeof(response),MSG_WAITALL);
        std::cout<<"recv response from server with flag: "<<response_len<<"\n";
        std::cout<<"recv response: \n";
        std::cout<<response<<"\n";
        
        if (response_len != 0) {
            PackResponse res(response);
            //string log_msg = generateLogMsg(thread_id,"Received \""+res.getFirstLine()
            //                        +"\" from "+request.getHostname());
            //writeToLog(log_msg);
            //cout << "received response: " << response << endl;
            int flag_s = send(fd_client, response, response_len, 0);
            std::cout<<"send request to client with flag: "<<flag_s<<"\n";
            //string log_msg2 = generateLogMsg(thread_id,"Responding \""+res.getFirstLine()+"\"");
            //writeToLog(log_msg2);
            std::cout << "post successfully\n";
        }
        else{
            std::cout<<"Server closed connection\n";
        }
    }
    else {
            std::cout << "failed to post\n";
    }

}

void Proxy::function400(int fd_client, int id) {
        const char * msg = "HTTP/1.1 400 Bad Request";
        send(fd_client, msg, sizeof(msg), MSG_NOSIGNAL);
        //pthread_mutex_lock(&mutex);
        //logFile << id << ": Responding \"HTTP/1.1 400 Bad Request\"" << std::endl;
        //pthread_mutex_unlock(&mutex);
}


/////////////test!!!
void posthandle(int fd_client, int fd_server, int thread_id, PackRequest * request_info) {
    std::cout<<send(fd_server, request_info->request.c_str(), request_info->request.size(), MSG_NOSIGNAL)<<std::endl;

    //h.recv_message(fd_server, &response, true);
    char response[65536] = {0};
    int response_len = recv(fd_server, response, sizeof(response), MSG_WAITALL);
    std::string response_info_s(response);

    std::cout<<"The Length is "<<response_len<<std::endl;
    //string response_str(response.begin(), response.end());
    if(response_len > 1){
        // Which parameter trans in.
        // response res;
        // res.parseResponse();
        //response_len = h.recv_message(server_fd, &response, false);
        std::cout<<response_len<<std::endl;
        std::string temp(response_info_s.begin(), response_info_s.begin() + response_len);
        std::cout<<temp<<std::endl;
        PackResponse res(temp); 
        //res.parseResponse();

        // How to get the first line in the response?
        //pthread_mutex_lock(&mutex);
        //logFile << thread_id << ": Received \"" << res.firstLine << "\" from " << request->uri << endl;
        //pthread_mutex_unlock(&mutex);
        send(fd_client, temp.c_str(), response_len, MSG_NOSIGNAL);

        //pthread_mutex_lock(&mutex);
        //logFile << thread_id << ": Responding \""<<res.firstLine<<endl;
        //pthread_mutex_unlock(&mutex);
    }
    else{
        std::cout<<"Server Socket Closed\n";
    }
}