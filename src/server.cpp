#include "storage/engine.hpp"
#include "storage/persistence.hpp"
#include "net/protocol.hpp"
#include <arpa/inet.h>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <unordered_map>
#include <cctype>
struct C{int fd;std::vector<uint8_t>in,out;};
static void nonblock(int f){fcntl(f,F_SETFL,fcntl(f,F_GETFL)|O_NONBLOCK);}
static std::string cmd(storage::StorageEngine&e,storage::Persistence&p,std::vector<std::string>a){if(a.empty())return"ERR empty";std::string c=a[0];for(char&x:c)x=std::toupper(x);try{
if(c=="PING")return"PONG";
if(c=="SET"&&a.size()>=3){long long t=-1;if(a.size()==5&&a[3]=="PX")t=std::stoll(a[4]);bool ok=e.set(a[1],a[2],t<0?std::nullopt:std::optional<std::chrono::milliseconds>(std::chrono::milliseconds(t)));if(ok)p.append_set(a[1],a[2],t);return ok?"OK":"EVICTED";}
if(c=="GET"&&a.size()==2){auto v=e.get(a[1]);return v?*v:"(nil)";}
if(c=="DEL"&&a.size()==2){bool ok=e.del(a[1]);if(ok)p.append_del(a[1]);return ok?"1":"0";}
if(c=="KEYS"){std::string r;for(auto&k:e.keys()){if(!r.empty())r+=' ';r+=k;}return r;}
if(c=="EXPIRE"&&a.size()==3)return e.expire(a[1],std::chrono::milliseconds(std::stoll(a[2])))?"1":"0";
if(c=="PTTL"&&a.size()==2)return std::to_string(e.pttl(a[1]));
if(c=="ZADD"&&a.size()==4){double s=std::stod(a[2]);bool ok=e.zadd(a[1],s,a[3]);if(ok)p.append_zadd(a[1],s,a[3]);return ok?"1":"0";}
if(c=="ZREM"&&a.size()==3){bool ok=e.zrem(a[1],a[2]);if(ok)p.append_zrem(a[1],a[2]);return ok?"1":"0";}
if(c=="ZSCORE"&&a.size()==3){auto s=e.zscore(a[1],a[2]);return s?std::to_string(*s):"(nil)";}
if(c=="ZQUERY"&&a.size()==5){auto r=e.zquery(a[1],std::stod(a[2]),std::stod(a[3]),std::stoull(a[4]));std::string x;for(auto&[m,s]:r){if(!x.empty())x+=' ';x+=m+":"+std::to_string(s);}return x;}
if(c=="INFO")return"keys="+std::to_string(e.size())+" memory="+std::to_string(e.memory_used());
if(c=="SAVE")return p.snapshot(e)?"OK":"ERR save";
}catch(const std::exception&x){return std::string("ERR ")+x.what();}return"ERR unknown";}
int main(int ac,char**av){int port=ac>1?std::stoi(av[1]):6380;storage::StorageEngine e;storage::Persistence p;p.open();p.replay(e);int s=socket(AF_INET,SOCK_STREAM,0),one=1;setsockopt(s,SOL_SOCKET,SO_REUSEADDR,&one,4);sockaddr_in a{};a.sin_family=AF_INET;a.sin_addr.s_addr=htonl(INADDR_ANY);a.sin_port=htons(port);if(bind(s,(sockaddr*)&a,sizeof(a))<0||listen(s,256)<0)return 1;nonblock(s);int ep=epoll_create1(0);epoll_event x{EPOLLIN};x.data.fd=s;epoll_ctl(ep,EPOLL_CTL_ADD,s,&x);std::unordered_map<int,C>cs;epoll_event ev[64];std::cout<<"listening "<<port<<"\n";while(1){int n=epoll_wait(ep,ev,64,-1);for(int i=0;i<n;i++){int f=ev[i].data.fd;if(f==s){while(1){int c=accept(s,0,0);if(c<0)break;nonblock(c);cs.emplace(c,C{c});epoll_event q{EPOLLIN};q.data.fd=c;epoll_ctl(ep,EPOLL_CTL_ADD,c,&q);}}else{auto it=cs.find(f);if(it==cs.end())continue;C&z=it->second;uint8_t b[4096];ssize_t r;while((r=recv(f,b,sizeof(b),0))>0)z.in.insert(z.in.end(),b,b+r);if(r==0){close(f);cs.erase(it);continue;}try{std::string q;while(net::extract(z.in,q)){auto o=net::frame(cmd(e,p,net::split(q)));z.out.insert(z.out.end(),o.begin(),o.end());}}catch(...){close(f);cs.erase(it);continue;}while(!z.out.empty()){ssize_t w=send(f,z.out.data(),z.out.size(),MSG_NOSIGNAL);if(w<=0)break;z.out.erase(z.out.begin(),z.out.begin()+w);}epoll_event q{EPOLLIN|(z.out.empty()?0:EPOLLOUT)};q.data.fd=f;epoll_ctl(ep,EPOLL_CTL_MOD,f,&q);}}}close(s);}
