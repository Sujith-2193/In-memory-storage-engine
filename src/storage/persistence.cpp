#include "storage/persistence.hpp"
#include <filesystem>
#include <utility>
namespace storage {
Persistence::Persistence(std::string d):dir_(std::move(d)),wal_(dir_+"/wal.log"),snap_(dir_+"/snapshot.txt"){}
bool Persistence::open(){std::filesystem::create_directories(dir_);out_.open(wal_,std::ios::app);return out_.good();}
static void put(std::ofstream&o,const std::string&s){o<<s.size()<<':'<<s;}
void Persistence::append_set(const std::string&k,const std::string&v,long long t){out_<<"S ";put(out_,k);out_<<' ';put(out_,v);out_<<' '<<t<<'\n';out_.flush();}
void Persistence::append_del(const std::string&k){out_<<"D ";put(out_,k);out_<<'\n';out_.flush();}
void Persistence::append_zadd(const std::string&k,double s,const std::string&m){out_<<"A ";put(out_,k);out_<<' '<<s<<' ';put(out_,m);out_<<'\n';out_.flush();}
void Persistence::append_zrem(const std::string&k,const std::string&m){out_<<"R ";put(out_,k);out_<<' ';put(out_,m);out_<<'\n';out_.flush();}
static bool tok(std::istream&i,std::string&o){std::size_t n;char c;if(!(i>>n>>c)||c!=':')return false;o.resize(n);i.read(o.data(),n);return(bool)i;}
bool Persistence::replay(StorageEngine&e){std::ifstream f(wal_);if(!f)return true;std::string op;while(f>>op){if(op=="S"){std::string k,v;long long t;if(!tok(f,k)||!tok(f,v)||!(f>>t))return false;e.apply_set(k,v,t);}else if(op=="D"){std::string k;if(!tok(f,k))return false;e.apply_del(k);}else if(op=="A"){std::string k,m;double s;if(!tok(f,k)||!(f>>s)||!tok(f,m))return false;e.apply_zadd(k,s,m);}else if(op=="R"){std::string k,m;if(!tok(f,k)||!tok(f,m))return false;e.apply_zrem(k,m);}}return true;}
bool Persistence::snapshot(const StorageEngine&e){std::ofstream f(snap_);if(!f)return false;for(auto&k:e.keys()){auto v=e.get(k);if(v){f<<"S ";put(f,k);f<<' ';put(f,*v);f<<" -1\n";}}return(bool)f;}
}