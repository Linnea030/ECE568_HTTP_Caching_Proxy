
#include "cache.h"

#define RESPONSE_MSG_MAX    65536

using namespace std;

mutex cache_mutex;

///////////

std::string getTime2() {
        time_t currTime =time(0);
        char * time_c = ctime(&currTime);
        //change it to UTC time zone
        std::tm * gm =gmtime(&currTime);
        time_c = asctime(gm);
        //make it as a string
        std::string time_s(time_c);
        return time_s.append("\0");
    }

// ID: not cacheable because REASON
// ID: cached, expires at EXPIRES
// ID: cached, but requires re-validation
void logCache2(int id , std::string reason, int mode, std::ofstream & file) {
        if(mode == 0) {
            file << id <<": not cacheable because "<< reason << std::endl;
        }
        else if(mode == 1) {
            std::string time = getTime2();
            file << id <<": cached, expires at "<< time << std::endl;
        }
        else if(mode == 2) {
            file<< id <<": in cache, requires validation"<< std::endl;
        }
}

    //ID: "REQUEST" from IPFROM @ TIME
    void logReq2(int id , std::string request_line, std::string ip_client, std::ofstream & file) {
        //test!!!
        std::string time = getTime2();
        file << id << ": \"" << request_line << "\" from "<< ip_client << " @ " << time;
        std::cout<<"\nafter log req\n";
    }

    //ID: Responding "RESPONSE"
    void logRes2(int id , std::string response_line, std::ofstream & file) {
        file << id << ": Responding \""<< response_line <<"\""<< std::endl;
    }

    //ID: not in cache
    //ID: in cache, but expired at EXPIREDTIME 
    //ID: in cache, requires validation
    //ID: in cache, valid
    void logGet2(int id , int mode, std::ofstream & file) {
        if(mode == 0) {
            file << id <<": not in cache"<< std::endl;
        }
        else if(mode == 1) {
            std::string time = getTime2();
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
    void logConServer2(int id, std::string re_line, std::string URI, int mode, std::ofstream & file) {
        if(mode == 0) {
            file << id << ": Requesting \"" << re_line << "\" from "<< URI << std::endl;
        }
        else if(mode == 1) {
            file << id << ": Received \"" << re_line << " \" from " << URI << std::endl;
        }
    }

    //ID: ERROR MESSAGE
    void logError2(std::string error, std::ofstream & file, int id = -1) {
        if(id = -1) {
            file << "(no-id): ERROR: "<< error << std::endl;
        }
        else{
            file << id <<": ERROR: "<< error << std::endl;
        }
    }

    //ID: WARNING MESSAGE
    void logWarning2(std::string warning, std::ofstream & file, int id = -1) {
        if(id = -1) {
            file << "(no-id): WARNING: "<< warning << std::endl;
        }
        else{
            file << id <<": WARNING: "<< warning << std::endl;
        }
    }

    //ID: NOTE MESSAGE
    void logNote2(std::string note, std::ofstream & file, int id = -1) {
        if(id = -1) {
            file << "(no-id): NOTE: "<< note << std::endl;
        }
        else{
            file << id <<": NOTE: "<< note << std::endl;
        }
    }

    //ID: Tunnel closed
    void logTunnel2(int id, std::ofstream & file) {
        std::cout<<"in log tunnel\n";
        file << id << ": Tunnel closed"<< std::endl;
        std::cout<<"out logtunnel\n";
    }
/////////

void Cache::addNode(Node * node){
    node->prev = head;
    node->next = head->next;
    head->next->prev = node;
    head->next = node;
    size++;
    if(size > total_size){
        //delete tail node(tail->prev)
        Node * node = tail->prev;
        tail->prev->prev->next = tail;
        tail->prev = tail->prev->prev;
        size--;
        map_cache.erase(node->uri);
        delete node;
    }
}

void Cache::moveToHead(Node * node){
    node->prev->next = node->next;
    node->next->prev = node->prev;
    head->next->prev = node;
    node->next = head->next;
    head->next = node;
    node->prev = head;
}

string Cache::search(string uri){
    lock_guard<mutex> lg(cache_mutex);
    // If in cache, return its value(response)
    if(map_cache.count(uri)){
        Node * node = map_cache[uri];
        // Move this node to the head of the list
        moveToHead(node);
        // Return stored response
        return node->value;
    }
    // If not in cache, return an empty string
    else{
        return "";
    }
}

void Cache::add(string uri, PackResponse response){
    lock_guard<mutex> lg(cache_mutex);
    // uri exists, change its value and move to head
    if(map_cache.count(uri)){
        Node * node = map_cache[uri];
        node->value = response.response;
        moveToHead(node);
    }
    // not exists, add a new node
    else{
        Node * node = new Node(uri, response.response);
        map_cache[uri] = node;
        addNode(node);
    }
}

void Cache::store(PackResponse response, int thread_id, string uri, std::ofstream & file, pthread_mutex_t & lock1){
    string cache_control = response.get_cachecontrol();
    bool isPrivate = false;
    bool isNoStore = false;
    std::cout<<"response code !!!!!!!!!!!!!!!!"<<response.code<<"!!!!!!"<<std::endl;
    //bool isNoCache = false;
    if(cache_control != ""){
        if(cache_control.find("private") != string::npos){
            isPrivate = true;
        }
        if(cache_control.find("no-store") != string::npos){
            isNoStore = true;
        }
        // if(cache_control.find("no-cache") != string::npos){
        //     isNoCache = true;
        // }
    }
    std::cout<<"isPrivate: "<<isPrivate<<" isNoStore: "<<isNoStore<<" response.code == \"200\": " <<(response.code == "200")<<
    " cache_control: "<<cache_control<<std::endl;
    if(!isPrivate && !isNoStore && response.code == "200"){
        // store in map_cache
        std::cout<<"in if before add\n";
        std::cout<<"uri: "<<uri<<"\nresponse:\n" <<response.response<<std::endl;
        add(uri, response);
        std::cout<<"in if after add\n";
        string expires = response.get_expires(); 
        std::cout<<"expires: "<<expires<<std::endl;
        if(expires == ""){
            std::cout<<"expires== \"\" re-validation "<<std::endl;
            //LOG: cached, but requires re-validation    
            //ID: in cache, requires validation
            pthread_mutex_lock(&lock1);
            logGet2(thread_id , 2, file);
            pthread_mutex_unlock(&lock1);
        }
        else{
            std::cout<<" expires at EXPIRES"<<std::endl;
            //LOG: cached, expires at EXPIRES
            //ID: in cache, but expired at EXPIREDTIME 
            pthread_mutex_lock(&lock1);
            logGet2(thread_id , 1, file);
            pthread_mutex_unlock(&lock1);
        }
    }
    if(isPrivate){
        //LOG: not cacheable because is private
        //ID: not cacheable because REASON
        std::string reason = "is private";
        pthread_mutex_lock(&lock1);
        logCache2(thread_id ,reason, 0, file);
        pthread_mutex_unlock(&lock1);
    }
    if(isNoStore){
        //LOG: not cacheable because is no-store
        std::string reason = "is no-store";
        pthread_mutex_lock(&lock1);
        logCache2(thread_id ,reason, 0, file);
        pthread_mutex_unlock(&lock1);
    }
    if(response.code != "200"){
        //LOG: not cacheable because not 200 OK
        std::string reason = "not 200 OK";
        pthread_mutex_lock(&lock1);
        logCache2(thread_id ,reason, 0, file);
        pthread_mutex_unlock(&lock1);
    }
    return;
}

string Cache::getResfromServer(PackRequest request, int fd_server){
 send(fd_server, request.request.data(), request.request.size(), MSG_NOSIGNAL);
    vector<char> msg;
    msg.resize(RESPONSE_MSG_MAX);
    int msg_len = recv(fd_server, &msg.data()[0], msg.size(), 0);
    int index = msg_len;
    PackResponse res(msg);
    if(msg_len <= 0){
        perror("Response msg recv error");
        return res.response;
    }
    if(res.code == "304"){
        return res.response;
    }
    // If chunked
    if(res.is_chunked()){
        cout<<"in is chunked: \n";
        string tmp;
        tmp.assign(msg.begin(), msg.end());
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
        cout<<"in is not chunked: \n";
        int len;
        len = res.get_length();
        // Receive until index = len
        while(len > index){
            msg.resize(index + RESPONSE_MSG_MAX);
            msg_len = recv(fd_server, &msg.data()[index], RESPONSE_MSG_MAX, 0);
            if(msg_len <= 0){
                break;
            }
            index = index + msg_len;
        }
    }
    string response = string(msg.begin(), msg.end());
    return response;
}

void Cache::revalidate(PackRequest request, int fd_client, int fd_server, string uri, PackResponse response, int thread_id, std::ofstream & file, pthread_mutex_t & lock1){
    string last_modified = response.get_lastmodified();
    string etag = response.get_etag();
    // Send request to server and receive
    if(etag == "" && last_modified == ""){
        PackResponse res(getResfromServer(request, fd_server));
        store(res, thread_id, uri, file, lock1);
        //Send response to remote client
        cout<<"response size"<<res.response.size()<<"\n";
        int flag_s;
        // LOG: Responding "RESPONSE"
        //ID: Responding "RESPONSE"
        pthread_mutex_lock(&lock1);
        logRes2(thread_id, res.response_line, file);
        pthread_mutex_unlock(&lock1);
        
        flag_s = send(fd_client, res.response.data(), res.response.size(), MSG_NOSIGNAL);
        cout<<"response_whole : \n"<<res.response.data()<<"\n";
        cout<<"send request to remote client with flag: "<<flag_s<<"\n";
    }
    if(etag != ""){
        string req = request.request_header;
        req += "\r\nIf-None-Match: " + etag + "\r\n\r]\n";
        cout << "Etag request: " << req << endl;
        PackResponse res(getResfromServer(req, fd_server));
        // send(fd_server, req.data(), req.size(), MSG_NOSIGNAL);
        if(res.code != "304"){
            store(res, thread_id, uri, file, lock1);
        }
        cout<<"response size"<<res.response.size()<<"\n";
        int flag_s;
        // LOG: Responding "RESPONSE"
        //ID: Responding "RESPONSE"
        pthread_mutex_lock(&lock1);
        logRes2(thread_id, res.response_line, file);
        pthread_mutex_unlock(&lock1);
        flag_s = send(fd_client, res.response.data(), res.response.size(), MSG_NOSIGNAL);
        cout<<"response_whole : \n"<<res.response.data()<<"\n";
        cout<<"send request to remote client with flag: "<<flag_s<<"\n";
    }
    if(last_modified != ""){
        string req = request.request_header;
        req += "\r\nIf-Modified-Since: " + last_modified + "\r\n\r]\n";
        cout << "Last-Modified request: " << req << endl;
        PackResponse res(getResfromServer(req, fd_server));
        // send(fd_server, req.data(), req.size(), MSG_NOSIGNAL);
        if(res.code != "304"){
            store(res, thread_id, uri, file, lock1);
        }
        cout<<"response size"<<res.response.size()<<"\n";
        int flag_s;
        // LOG: Responding "RESPONSE"
        //ID: Responding "RESPONSE"
        pthread_mutex_lock(&lock1);
        logRes2(thread_id, res.response_line, file);
        pthread_mutex_unlock(&lock1);
        flag_s = send(fd_client, res.response.data(), res.response.size(), MSG_NOSIGNAL);
        cout<<"response_whole : \n"<<res.response.data()<<"\n";
        cout<<"send request to remote client with flag: "<<flag_s<<"\n";
    }
    return;
}
