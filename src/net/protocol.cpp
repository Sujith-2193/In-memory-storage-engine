#include "net/protocol.hpp"
#include <cstring>
#include <sstream>
#include <stdexcept>
namespace net {
std::vector<std::uint8_t> frame(const std::string&p){std::uint32_t n=p.size();std::vector<std::uint8_t>b(4+n);b[0]=n>>24;b[1]=n>>16;b[2]=n>>8;b[3]=n;std::memcpy(b.data()+4,p.data(),n);return b;}
bool extract(std::vector<std::uint8_t>&b,std::string&p){if(b.size()<4)return false;std::uint32_t n=(b[0]<<24)|(b[1]<<16)|(b[2]<<8)|b[3];if(n>16*1024*1024)throw std::runtime_error("frame too large");if(b.size()<4+n)return false;p.assign((char*)b.data()+4,n);b.erase(b.begin(),b.begin()+4+n);return true;}
std::vector<std::string>split(const std::string&s){std::istringstream in(s);std::vector<std::string>r;std::string x;while(in>>x)r.push_back(x);return r;}}
