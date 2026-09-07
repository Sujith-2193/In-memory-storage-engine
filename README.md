# High-Performance In-Memory Storage Engine

A C++17 storage server built from first principles to study systems programming: non-blocking sockets, Linux `epoll`, binary framing, hash tables, ordered indexes, TTLs, LRU eviction, write-ahead logging, snapshots, and recovery.

## Features

- TCP server using non-blocking sockets and an `epoll` event loop
- 4-byte big-endian length-prefixed request/response framing
- `PING`, `GET`, `SET`, `DEL`, `KEYS`, `EXPIRE`, `PTTL`
- Sorted-set commands: `ZADD`, `ZREM`, `ZSCORE`, `ZQUERY`
- Custom hash-table backed key store
- Ordered `std::map` index for sorted-set queries
- Thread-safe storage API with explicit ownership and locking
- Configurable memory limit with LRU eviction
- Append-only write-ahead log with replay on startup
- Snapshot command (`SAVE`)
- CLI client and unit tests
- Linux/Unix build with a small Makefile

## Protocol

Each request is a binary frame:

```text
+----------------------+--------------------+
| uint32 payload_size  | command bytes      |
| big endian           | payload_size bytes |
+----------------------+--------------------+
```

The payload is a whitespace-separated command. Responses use the same framing. This keeps transport framing independent from command semantics and handles partial TCP reads by buffering until a complete frame arrives.

## Commands

```text
PING
SET key value
SET key value PX milliseconds
GET key
DEL key
KEYS
EXPIRE key milliseconds
PTTL key
ZADD key score member
ZREM key member
ZSCORE key member
ZQUERY key min_score max_score limit
INFO
SAVE
```

## Data Structures & Complexity

| Operation | Expected complexity |
|---|---:|
| Hash-table GET/SET/DEL | O(1) average |
| KEYS | O(n log n) including deterministic sorting |
| LRU touch/eviction | O(1) |
| Sorted-set insertion | O(log n) |
| Sorted-set range scan | O(log n + k) for k returned entries |
| TTL lookup | O(1) |

The implementation deliberately exposes the trade-off between hash-based point lookup and ordered indexing for range queries.

## Persistence

Mutating commands append records to `data/wal.log` and flush them before returning. Startup replays the WAL to reconstruct in-memory state. `SAVE` writes a compact snapshot of string keys to `data/snapshot.txt` for inspection and recovery experiments. The WAL remains the authoritative mutation log.

## Build & Run

Linux is required for `epoll`.

```bash
make
./bin/storage_server 6380
```

In another terminal:

```bash
printf 'SET user 42\nGET user\nINFO\n' | ./bin/storage_client
```

Run tests:

```bash
./bin/storage_tests
```

Memory limit is configured in the engine API; the default server limit is 64 MiB.

## Architecture

```text
TCP Client
    |
    v
+---------------------------+
| Non-blocking TCP Server   |
|        epoll loop         |
+-------------+-------------+
              |
              v
       Frame Decoder
              |
              v
       Command Processor
          /         \
         v           v
   Hash Table      Sorted Set
      |             ordered map
      +-------> Storage Engine
                    |
               WAL / Snapshot
```

## Engineering Focus

This project is intentionally lower-level than a typical CRUD application. The important learning surface is the boundary between the OS and the data structure layer: file descriptors, readiness notifications, partial I/O, protocol framing, memory accounting, eviction policy, persistence ordering, and recovery.
