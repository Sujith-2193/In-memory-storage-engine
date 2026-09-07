#include "storage/engine.hpp"
#include <cassert>
#include <chrono>
#include <thread>
int main(){storage::StorageEngine e(128);assert(e.set("a","one"));assert(e.get("a")=="one");assert(e.del("a"));assert(!e.get("a"));e.set("ttl","x",std::chrono::milliseconds(20));assert(e.pttl("ttl")>0);std::this_thread::sleep_for(std::chrono::milliseconds(30));assert(!e.get("ttl"));e.zadd("rank",2.0,"bob");e.zadd("rank",1.0,"alice");assert(e.zscore("rank","bob")==2.0);auto r=e.zquery("rank",0,1.5,10);assert(r.size()==1&&r[0].first=="alice");storage::StorageEngine small(8);small.set("a","123456");small.set("b","123456");assert(!small.get("a")||small.get("b"));return 0;}
