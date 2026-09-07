#pragma once
#include <chrono>
#include <cstddef>
#include <list>
#include <map>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
namespace storage {
struct StringValue { std::string value; };
struct SortedSetValue { std::map<std::pair<double,std::string>,double> entries; };
using Value=std::variant<StringValue,SortedSetValue>;
struct Entry { Value value; std::optional<std::chrono::steady_clock::time_point> expires_at; std::size_t bytes{0}; };
class StorageEngine {
public:
 explicit StorageEngine(std::size_t max_memory=64*1024*1024);
 bool set(const std::string&,const std::string&,std::optional<std::chrono::milliseconds> ttl=std::nullopt);
 std::optional<std::string> get(const std::string&); bool del(const std::string&); std::vector<std::string> keys();
 bool expire(const std::string&,std::chrono::milliseconds); long long pttl(const std::string&);
 bool zadd(const std::string&,double,const std::string&); bool zrem(const std::string&,const std::string&);
 std::optional<double> zscore(const std::string&,const std::string&);
 std::vector<std::pair<std::string,double>> zquery(const std::string&,double,double,std::size_t);
 std::size_t size() const; std::size_t memory_used() const; std::size_t max_memory() const; void clear();
 bool apply_set(const std::string&,const std::string&,long long); bool apply_del(const std::string&);
 bool apply_zadd(const std::string&,double,const std::string&); bool apply_zrem(const std::string&,const std::string&);
private:
 using LruList=std::list<std::string>; struct Node{Entry entry;LruList::iterator lru_it;};
 bool expired(const Node&) const; void touch(Node&); void erase_unlocked(const std::string&); void evict_if_needed();
 static std::size_t value_bytes(const Value&); Node* find_unlocked(const std::string&);
 mutable std::mutex mu_; std::unordered_map<std::string,Node> table_; LruList lru_; std::size_t memory_used_{0},max_memory_;
};
}