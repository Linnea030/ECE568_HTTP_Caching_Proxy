#include "package.h"

void PackRequest::parse_req() {
    this->parse_line();
    this->parse_header();
    //this->parse_body();
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
    size_t pos_line = request.find("\r\n");
    size_t pos_header = request.find("\r\n\r\n");
    if(pos_header == std::string::npos) {
        port = "";
        hostname = "";
        //判断！！！
        return;
    }
	request_header = request.substr(pos_line + strlen("\r\n"), pos_header);
    size_t pos_host = request.find("Host");
	std::string mid_host = request.substr(pos_host+6);
	size_t end = mid_host.find("\r\n");
	std::string host_port = mid_host.substr(0,end);
	size_t port_pos = host_port.find(":");
	//std::cout << port_pos <<std::endl;
	if(port_pos!= std::string::npos){
		hostname = host_port.substr(0,port_pos);
		port = host_port.substr(port_pos+1);
	} else{
		hostname = host_port;
		port="80";
	}
    // //get hostname and port
    // size_t pos_h = request_header.find("Host") + 6;
    // //std::cout<<request_header<<"\n\n";
    // size_t pos_h_end = request_header.find("\r\n\r\n");
    // std::cout<<"pos_h_end: "<<pos_h_end<<"\n";
	// std::string line_hp = request_header.substr(pos_h, pos_h_end - pos_h);
    // //std::cout<<line_hp<<"\n\n";
    // //get port
	// size_t pos_p = line_hp.find(":");
    // //std::cout<<"!!!!!!!!!"<<line_hp<<"\n";
	// if(pos_p == std::string::npos){
    //     std::cout<<"port is 80\n";
    //     port = "80";
	// 	hostname = line_hp;
	// } else{
    //     std::cout<<"port is not 80\n";
	// 	port = line_hp.substr(pos_p + 1);
    //     //std::cout<<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
    //     //std::cout<<port.size()<<"\n";
    //     hostname = line_hp.substr(0, pos_p);
	// }
}

void PackRequest::parse_body() {
    size_t pos_header = request.find("\r\n\r\n");
	//if(method == "POST"){
		std::string info = request.substr(pos_header + 4);
		size_t pos_len = request_header.find("Content-Length");
        if (pos_len == std::string::npos) {
            content_len_remain = -1;
        }
		std::string content_len;
        content_len = request_header.substr(pos_len + 16);
		size_t pos_len_end = content_len.find("\r\n");
		len_info = content_len.substr(0, pos_len_end);
	//}

    int rest_len = request.size() - int(pos_header) - 8;
    size_t end = request.find("\r\n", pos_len);
    //len_info = stoi(request.substr(pos_len + 16, end - pos_len - 16));
    content_len_remain = stoi(len_info) - rest_len - 4;
}

void PackRequest::parse_cache() {
  size_t pos_noc = request.find("no-cache");
  if (pos_noc == std::string::npos) {
    flag_nocache = 0;
  }
  else {
    flag_nocache = 1;
  }
}

void PackRequest::print_request() {
    std::cout<<"Here is request info:\n";
    std::cout<<"request_line: " << request_line <<"\n";
    std::cout<<"request_header: " << request_header <<"\n";
    //std::cout<<"request_body: " << request_body <<"\n";
    //std::cout<<"request_URI: " << URI <<"\n";
    std::cout<<"request_method: " << method <<"\n";
    std::cout<<"request_hostname: " << hostname <<"\n";
    std::cout<<"request_port: " << port <<"\n";
    std::cout<<"len_info: " << len_info <<"\n";
    //std::cout<<"request_info: " << info <<"\n";
}

//========== Response ==========

void PackResponse::parse_res() {
    parse_header();
    parse_status();
}

void PackResponse::parse_header(){
    size_t pos_h_end = response.find("\r\n\r\n");
    response_header = response.substr(0, pos_h_end);
    response_body = response.substr(pos_h_end + 4);
}

void PackResponse::parse_status(){
    size_t pos_st = response_header.find(" ");
    size_t pos_code_end = response_header.find(" ");
    size_t pos_st_end = response_header.find("\r\n");
    status_code = response_header.substr(pos_st, pos_st_end);
    code = response_header.substr(pos_st, pos_code_end);
}

bool PackResponse::is_chunked(){
    //size_t pos = response.find("chunked");
    // if(response.find("chunked") != std::string::npos){
    //     return true;
    // }
    return (response.find("chunked") != std::string::npos) ? true : false;
}

int PackResponse::get_length(){
    std::string name = "Content-Length";
    size_t pos = response.find(name);
    //if not found?
    size_t pos_end = response.find("\r\n", pos + 1);
    std::string len_s = response.substr(pos + 2 + name.size(), pos_end - pos - name.size() - 2);
    std::cout<<"in get_length, len_s: "<<len_s<<"\n";
    return stoi(len_s);
}
