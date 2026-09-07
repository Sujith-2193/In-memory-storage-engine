#pragma once
#include "storage/engine.hpp"
#include <fstream>
#include <string>
namespace storage {
class Persistence{public: explicit Persistence(std::string dir="data"); bool open(); void append_set(const std::string&,const std::string&,long long); void append_del(const std::string&); void append_zadd(const std::string&,double,const std::string&); void append_zrem(const std::string&,const std::string&); bool replay(StorageEngine&); bool snapshot(const StorageEngine&); private: std::string dir_,wal_,snap_; std::ofstream out_;};
}