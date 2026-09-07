# In-Memory Storage Engine

A C++17 event-driven in-memory storage engine built from first principles to explore systems programming, networking, data structures, caching, expiration, persistence, and recovery.

## Current Scope

- TCP client/server architecture
- Non-blocking event loop with Linux `epoll`
- Length-prefixed binary protocol
- `GET`, `SET`, `DEL`, `KEYS`
- Key expiration with TTL
- Custom hash-table storage
- AVL-tree based sorted sets
- Command-oriented request processing
- Graceful shutdown and connection lifecycle management

## Planned Extensions

- LRU eviction with configurable memory limits
- Write-ahead logging (WAL)
- Periodic snapshots and crash recovery
- Concurrent stress testing and benchmarking
- Performance metrics and latency analysis

## Why This Project

The goal is to understand how an in-memory storage service works below the application-framework level: sockets, file descriptors, event-driven I/O, protocol framing, indexing, expiration, eviction, persistence, and recovery.

## Build

Linux is required for the `epoll` implementation.

```bash
make
```

## Run

Start the server:

```bash
./bin/storage_server
```

Then connect using the included client:

```bash
./bin/storage_client
```

## Architecture

```text
Client
  │
  │ TCP
  ▼
Storage Server
  │
  ├── epoll Event Loop
  ├── Connection Manager
  ├── Binary Protocol
  ├── Command Processor
  └── Storage Engine
        ├── Hash Table
        └── AVL Tree
```

## Design Principles

- Keep the data plane simple and explicit.
- Prefer deterministic ownership and lifetime of resources.
- Separate networking, protocol parsing, commands, and storage.
- Document complexity and trade-offs alongside implementation.
