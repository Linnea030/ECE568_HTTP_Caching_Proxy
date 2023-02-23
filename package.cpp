#include "package.h"

void PackRequest::parse() {
    this->parse_line();
    this->parse_header();
    this->parse_body();
}

//parse line to get method and url
void PackRequest::parse_line() {
    //get request line
    size_t pos_line = request.find("\r\n");
	request_line = request.substr(0, pos_line);
    //get method in line
    size_t pos_m = request_line.find(" ");
	method = request_line.substr(0, pos_m);
    //get URI in line
	size_t pos_u = request_line.find(" ", pos_m + 1);
	URI = request_line.substr(pos_m + 1, pos_u - pos_m);
}

void PackRequest::parse_header() {
    //get request header
    size_t pos_header = request.find("\r\n\r\n");
    if(pos_header == std::string::npos) {
        port = "";
        hostname = "";
        //判断！！！
        return;
    }
	request_header = request.substr(0, pos_header);
    //get hostname and port
    size_t pos_h = request_header.find("Host") + 6;
    size_t pos_h_end = request_header.find("\r\n");
	std::string line_hp = request_header.substr(pos_h, pos_h_end -pos_h);
    //get port
	size_t pos_p = line_hp.find(":");
	if(pos_p == std::string::npos){
        port="80";
		hostname = line_hp;
	} else{
		port = line_hp.substr(pos_p + 1);
        hostname = line_hp.substr(0, pos_p);
	}
}

void PackRequest::parse_body() {

}

void PackRequest::print_request() {
    std::cout<<"Here is request info:\n";
    std::cout<<"request_line: " << request_line <<"\n";
    std::cout<<"request_header: " << request_header <<"\n";
    //std::cout<<"request_body: " << request_body <<"\n";
    std::cout<<"request_URI: " << URI <<"\n";
    std::cout<<"request_method: " << method <<"\n";
    std::cout<<"request_hostname: " << hostname <<"\n";
    std::cout<<"request_port: " << port <<"\n";
    //std::cout<<"request_info: " << info <<"\n";
}