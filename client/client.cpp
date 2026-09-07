#include "net/protocol.hpp"
#include <arpa/inet.h>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
int main(int ac,char**av){const char*h=ac>1?av[1]:"127.0.0.1";int port=ac>2?std::stoi(av[2]):6380;int s=socket(AF_INET,SOCK_STREAM,0);sockaddr_in a{};a.sin_family=AF_INET;a.sin_port=htons(port);inet_pton(AF_INET,h,&a.sin_addr);if(connect(s,(sockaddr*)&a,sizeof(a))<0)return 1;std::string line;std::vector<uint8_t>buf;while(std::getline(std::cin,line)){auto f=net::frame(line);send(s,f.data(),f.size(),0);uint8_t b[4096];ssize_t n=recv(s,b,sizeof(b),0);if(n<=0)break;buf.insert(buf.end(),b,b+n);std::string out;while(net::extract(buf,out))std::cout<<out<<"\n";}close(s);}
