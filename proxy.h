#ifndef _PROXY_
#define _PROXY_
#include <pthread.h>
#include "csbuild.h"
#include "package.h"
//#include "handler.h"
#include "socket_info.h"

class Proxy {
    public:
    const char * port;
    //static std::ofstream file;

    // ////////////////not necessary
    int fd_accept;//used to accept remote client
    // int fd_client;//used to send and recv from remote client
    // int fd_server;//used to connect remote server
    // std::string ip_client;//ip for remote client used to print in log file
    //Csbuild cs;

    //Proxy(const char * port) : port(port), fd_accept(-1), fd_client(-1), fd_server(-1), ip_client("") {}
    Proxy(const char * port) : port(port) {
        //file.open("proxy.log");
    }
    void init_Proxy();
    static void connect_function(int fd_client, int fd_server, int id, SocketInfo * thread_info);
    //void update_cs();
    static void post_function(int fd_client, int fd_server, int id, PackRequest & request_info, SocketInfo * thread_info);
    static void function400(int fd_client, int id);
    static void * process(void * thread1);


////////////////////

    // static std::string getTime() {
    //     time_t currTime =time(0);
    //     char * time_c = ctime(&currTime);
    //     //change it to UTC time zone
    //     std::tm * gm =gmtime(&currTime);
    //     time_c = asctime(gm);
    //     //make it as a string
    //     std::string time_s(time_c);
    //     return time_s;
    // }

    // //ID: "REQUEST" from IPFROM @ TIME
    // static void logReq(int id , std::string request_line, std::string ip_client) {
    //     //test!!!
    //     std::cout<<"in log req\n";
    //     std::string time = getTime();
    //     file << id << ": \"" << request_line << "\" from "<< ip_client << " @ " << time;
    // }

    // //ID: Responding "RESPONSE"
    // static void logRes(int id , std::string response_line) {
    //     file << id << ": Responding \""<< response_line <<"\"\n";
    // }

    // //ID: not in cache
    // //ID: in cache, but expired at EXPIREDTIME 
    // //ID: in cache, requires validation
    // //ID: in cache, valid
    // static void logGet(int id , int mode) {
    //     if(mode == 0) {
    //         file << id <<": not in cache\n";
    //     }
    //     else if(mode == 1) {
    //         std::string time = getTime();
    //         file << id <<": in cache, but expired at "<< time <<"\n";
    //     }
    //     else if(mode == 2) {
    //         file<< id <<": in cache, requires validation\n";
    //     }
    //     else if(mode == 3) {
    //         file<< id <<": in cache, valid\n";
    //     }
    // }

    // //ID: Requesting "REQUEST" from SERVER
    // //ID: Received "RESPONSE" from SERVER
    // static void logConServer(int id, std::string re_line, std::string URI, int mode) {
    //     if(mode == 0) {
    //         file << id << ": Requesting \"" << re_line << "\" from "<< URI <<"\n";
    //     }
    //     else if(mode == 1) {
    //         file << id << ": Received \"" << re_line << " \" from " << URI <<"\n";
    //     }
    // }

    // //ID: ERROR MESSAGE
    // static void logError(std::string error, int id = -1) {
    //     if(id = -1) {
    //         file << "(no-id): ERROR: "<< error <<"\n";
    //     }
    //     else{
    //         file << id <<": ERROR: "<< error <<"\n";
    //     }
    // }

    // //ID: WARNING MESSAGE
    // static void logWarning(std::string warning, int id = -1) {
    //     if(id = -1) {
    //         file << "(no-id): WARNING: "<< warning <<"\n";
    //     }
    //     else{
    //         file << id <<": WARNING: "<< warning <<"\n";
    //     }
    // }

    // //ID: NOTE MESSAGE
    // static void logNote(std::string note, int id = -1) {
    //     if(id = -1) {
    //         file << "(no-id): NOTE: "<< note <<"\n";
    //     }
    //     else{
    //         file << id <<": NOTE: "<< note <<"\n";
    //     }
    // }

    // //ID: Tunnel closed
    // static void logTunnel(int id) {
    //     file << id << ": Tunnel closed\n";
    // }

//////////
    // void proxy_print(){
    //     std::cout<<"info of proxy:\n";
    //     std::cout<<"fd_server: "<<fd_server<<" fd_accept: "<<fd_accept<<" fd_client: "<<fd_client<<" ip_client: "<<ip_client<<"\n";
    //     std::cout<<"\n";
    // }

    //void * processCONNECT();

    ~Proxy() {
        // close(fd_server);
        // close(fd_accept);
        // close(fd_client);
    }
};

#endif