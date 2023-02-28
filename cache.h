#ifndef _CACHE_
#define _CACHE_
#include <map>
#include <string>
#include <mutex>
#include "package.h"
#include <sys/socket.h>

using namespace std;

class Cache{
    public:
    class Node{
        public:
        string uri;
        string value;
        Node * prev;
        Node * next;
        Node():prev(NULL), next(NULL){}
        Node(string u, string val): uri(u), value(val), prev(NULL), next(NULL){}
    };
    map<string, Node*> map_cache;   //map to store cache
    int size;           //currsent size
    int total_size;     //total size
    Node * head;
    Node * tail;

    Cache(){}
    Cache(int ts): total_size(ts), size(0){
        head = new Node();
        head->value = "HEAD";
        tail = new Node(); 
        tail->value = "TAIL";
        head->next = tail;
        tail->prev = head;
    }

    ~Cache(){}
    
    void moveToHead(Node * node);
    void addNode(Node * node);

    string search(string uri);
    void add(string uri, PackResponse response);
    void store(PackResponse response, int thread_id, string uri, std::ofstream & file);
    string getResfromServer(PackRequest request, int fd_server);
    void revalidate(PackRequest request, int fd_client, int fd_server, string uri, PackResponse response, int thread_id, std::ofstream & file);
};
#endif
