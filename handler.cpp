#include "handler.h"
#include "package.h"
#include "socket_info.h"
#include "csbuild.h"
#include "cache.h"
//#include "log.h"
//#include "helper.h"

#define RESPONSE_MSG_MAX    65536

using namespace std;

///////////
std::string getTime1() {
        time_t currTime =time(0);
        char * time_c = ctime(&currTime);
        //change it to UTC time zone
        std::tm * gm =gmtime(&currTime);
        time_c = asctime(gm);
        //make it as a string
        std::string time_s(time_c);
        return time_s.append("\0");
    }

    //ID: "REQUEST" from IPFROM @ TIME
    void logReq1(int id , std::string request_line, std::string ip_client, std::ofstream & file) {
        //test!!!
        std::string time = getTime1();
        file << id << ": \"" << request_line << "\" from "<< ip_client << " @ " << time;
        std::cout<<"\nafter log req\n";
    }

    //ID: Responding "RESPONSE"
    void logRes1(int id , std::string response_line, std::ofstream & file) {
        file << id << ": Responding \""<< response_line <<"\""<< std::endl;
    }

    //ID: not in cache
    //ID: in cache, but expired at EXPIREDTIME 
    //ID: in cache, requires validation
    //ID: in cache, valid
    void logGet1(int id , int mode, std::ofstream & file) {
        if(mode == 0) {
            file << id <<": not in cache"<< std::endl;
        }
        else if(mode == 1) {
            std::string time = getTime1();
            file << id <<": in cache, but expired at "<< time << std::endl;
        }
        else if(mode == 2) {
            file<< id <<": in cache, requires validation"<< std::endl;
        }
        else if(mode == 3) {
            file<< id <<": in cache, valid"<< std::endl;
        }
    }

    //ID: Requesting "REQUEST" from SERVER
    //ID: Received "RESPONSE" from SERVER
    void logConServer1(int id, std::string re_line, std::string URI, int mode, std::ofstream & file) {
        if(mode == 0) {
            file << id << ": Requesting \"" << re_line << "\" from "<< URI << std::endl;
        }
        else if(mode == 1) {
            file << id << ": Received \"" << re_line << " \" from " << URI << std::endl;
        }
    }

    //ID: ERROR MESSAGE
    void logError1(std::string error, std::ofstream & file, int id = -1) {
        if(id = -1) {
            file << "(no-id): ERROR: "<< error << std::endl;
        }
        else{
            file << id <<": ERROR: "<< error << std::endl;
        }
    }

    //ID: WARNING MESSAGE
    void logWarning1(std::string warning, std::ofstream & file, int id = -1) {
        if(id = -1) {
            file << "(no-id): WARNING: "<< warning << std::endl;
        }
        else{
            file << id <<": WARNING: "<< warning << std::endl;
        }
    }

    //ID: NOTE MESSAGE
    void logNote1(std::string note, std::ofstream & file, int id = -1) {
        if(id = -1) {
            file << "(no-id): NOTE: "<< note << std::endl;
        }
        else{
            file << id <<": NOTE: "<< note << std::endl;
        }
    }

    //ID: Tunnel closed
    void logTunnel1(int id, std::ofstream & file) {
        std::cout<<"in log tunnel\n";
        file << id << ": Tunnel closed"<< std::endl;
        std::cout<<"out logtunnel\n";
    }
/////////

/**
 * @brief Handle GET method
 * 
 * @param request: client request info
 * @param fd_client: remote client fd
 * @param fd_server: remote server fd
 * 
 * @param cache: cache相关info
 * 
 * @param thread_id: 线程id
 * 
 * @return null
 */
void Handler::GETHandler(PackRequest req, int fd_client, int fd_server, Cache & cache, int thread_id, std::ofstream & file, pthread_mutex_t & lock1){
    std::cout<<"int get handler\n";
    //string hostname = req.hostname;
    //string portnum = req.port;
    string uri = req.URI; //判断cache
    // 作为client与remote server连接: fd_server
    // int client_fd;
    // client_fd = init_client(hostname, portnum);
    // 先在cache里面找
    // 待补充：在cache里面
    //bool in_cache = false;
    string cache_res = cache.search(uri);
    if(cache_res != ""){
        PackResponse res = PackResponse(cache_res);
        //Check if requires re-validate
        string cache_control = res.get_cachecontrol();
        bool isNoCache = false;
        if(cache_control.find("no-cache") != string::npos){
            isNoCache = true;
        }
        //待补充：maxage等
        if(isNoCache){
            //LOG: cached, requires validation
            //ID: in cache, requires validation
            pthread_mutex_lock(&lock1);
            logGet1(thread_id , 2, file);
            pthread_mutex_unlock(&lock1);
            cache.revalidate(req, fd_client, fd_server, uri, res, thread_id, file, lock1);
            return;
        }
        else{
            // LOG: in cache, valid
            cout<<"response size"<<res.response.size()<<"\n";
            int flag_s;
            // LOG: Responding "RESPONSE"
            flag_s = send(fd_client, res.response.data(), res.response.size(), MSG_NOSIGNAL);
            cout<<"response_whole : \n"<<res.response.data()<<"\n";
            cout<<"send request to remote client with flag: "<<flag_s<<"\n";
        }
    }
    // If not in cache
    else{
        std::cout<<"not in cache\n";
        //待补充：写入log: not in cache
        //ID: not in cache
        pthread_mutex_lock(&lock1);
        logGet1(thread_id , 0, file);
        pthread_mutex_unlock(&lock1);
        //读取client的request，send给remote server
        //待补充：写入log：request from server
        //ID: Requesting "REQUEST" from SERVER
        pthread_mutex_lock(&lock1);
        logConServer1(thread_id, req.request_line, req.URI, 0, file);
        pthread_mutex_unlock(&lock1);
        //client接受返回消息：判断chunk！
        //处理response
        //send回client

        //send request to remote server
        int flag_s;
        std::cout<<"req.request.data():\n"<<req.request.data()<<"\n";
        flag_s = send(fd_server, req.request.data(), req.request.size(), MSG_NOSIGNAL);
        std::cout<<"send request to remote server with flag: "<<flag_s<<"\n";

        //receive response
        vector<char> msg;
        msg.resize(RESPONSE_MSG_MAX);
        // receive the first response package
        int msg_len = recv(fd_server, &msg.data()[0], msg.size(), 0);
        if(msg_len <= 0){
            perror("Response msg recv error");
            return;
        }
        std::cout<<"recv response from remote server with flag: "<<msg_len<<"\n";

        int index = msg_len;
        // 待补充：把第一个包存入response中
        //string response_info_s(msg);
        PackResponse res(msg);
        //ID: Received "RESPONSE" from SERVER
        pthread_mutex_lock(&lock1);
        logConServer1(thread_id, res.response_line, res.URI, 1, file);
        pthread_mutex_unlock(&lock1);

        //处理code 304？
        if(res.code == "304"){
            //ID: NOTE MESSAGE
            std::string warning = "get 304 not modified";
            pthread_mutex_lock(&lock1);
            logNote1(warning, file, thread_id);
            pthread_mutex_unlock(&lock1);
            return;
        }

        // 待补充：if res.is_chunked()
        if(res.is_chunked()){
            std::cout<<"in is chunked: \n";
            string tmp;
            //tmp.insert(tmp.begin(), msg.begin(), msg.end());
            tmp.assign(msg.begin(), msg.end());
            // 这个边界条件什么意思？ Chunk结束条件？
            while(tmp.find("0\r\n\r\n") == string::npos){
                msg.resize(index + RESPONSE_MSG_MAX);
                msg_len = recv(fd_server, &msg.data()[index], RESPONSE_MSG_MAX, 0);
                if(msg_len <= 0){
                    break;
                }
                tmp.assign(msg.begin() + index, msg.end() + index);
                index = index + msg_len;
            }
        }
        // If not chunked
        else{
            std::cout<<"in is not chunked: \n";
            int len;
            // 待补充：len = res.getLen();
            len = res.get_length();
            // Receive until index = len
            while(len > index){
                msg.resize(index + RESPONSE_MSG_MAX);
                // recv size应为65536还是msg.size()?
                msg_len = recv(fd_server, &msg.data()[index], RESPONSE_MSG_MAX, 0);
                if(msg_len <= 0){
                    break;
                }
                index = index + msg_len;
            }
        }

        // 待补充：把结果存入response中
        // string response_info_s(msg);
        PackResponse response(msg);

        //ID: Received "RESPONSE" from SERVER
        pthread_mutex_lock(&lock1);
        logConServer1(thread_id, response.response_line, response.URI, 1, file);
        pthread_mutex_unlock(&lock1);

        // 待补充：response存入cache
        cache.store(response, thread_id, uri, file, lock1);

        //Send response to remote client
        string response_whole;
        response_whole = string(msg.begin(), msg.end());
        std::cout<<"response size"<<response_whole.size()<<"\n";
        flag_s = send(fd_client, response_whole.data(), response_whole.size(), MSG_NOSIGNAL);
        std::cout<<"response_whole : \n"<<response_whole.data()<<"\n";
        std::cout<<"send request to remote client with flag: "<<flag_s<<"\n";
        
        //ID: Responding "RESPONSE"
        pthread_mutex_lock(&lock1);
        logRes1(thread_id, res.response_line, file);
        pthread_mutex_unlock(&lock1);

    }
    return;
}

    bool Handler::is502(std::string res) {
        if(res.find("HTTP/1.1")==string::npos){
            return true;
        }
        if(res.find("\r\n\r\n")==string::npos){  
            return true;
        }
        return false;
    }

    void Handler::function502(int fd_client, int id, std::ofstream & file, pthread_mutex_t & lock1) {
        const char * error = "HTTP/1.1 502 Bad Gateway";
        int flag;
        flag = send(fd_client, error, sizeof(error), 0);
        if(flag < 0){

        }
        pthread_mutex_lock(&lock1);
        logRes1(id, "HTTP/1.1 502 Bad Gateway", file);
        pthread_mutex_unlock(&lock1);
    }

        