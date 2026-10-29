# portscanner

A fast multithreaded TCP port scanner written from scratch in modern C++20.

```
$ portscanner 127.0.0.1 -p 2222,8000-8001
Порт: 2222 | Состояние: Открыт | SSH-2.0-OpenSSH_9.9
Порт: 8000 | Состояние: Открыт | SimpleHTTP/0.6 Python/3.14.7
Порт: 8001 | Состояние: Закрыт
```

## Features

- **Flexible port syntax**: single ports, ranges and lists: `22,80,8000-8100`
- **Three port states**: open, closed, filtered (a firewall silently drops packets)
- **Connect timeout**: 1 second instead of the default ~2 minutes for filtered ports
- **Parallel scanning**: 50 worker threads, no locks and no data races
- **Service detection**: reads service banners and asks HTTP servers for their `Server` header
- **RAII socket wrapper**: file descriptors are always closed, even on early return
- **Strict input validation**: rejects `abc`, `0`, `65536`, `80abc`, `8005-8000` and invalid IPs

## Benchmarks

| Scenario | Sequential | 50 threads |
|---|---|---|
| 100 filtered ports | 102 s | **2 s** |
| All 65 535 ports on localhost | — | **0.07 s** |

## Build

Requires CMake 3.20+ and a C++20 compiler (GCC 11+ or Clang 14+). Linux only.

```bash
cmake -S . -B build
cmake --build build
```

## Usage

```bash
./build/portscanner <target> -p <ports>
```

Examples:

```bash
./build/portscanner 127.0.0.1 -p 80
./build/portscanner 127.0.0.1 -p 1-1024
./build/portscanner 127.0.0.1 -p 22,80,443,8000-8100
```

## How it works

1. `parse_list` splits the port string by commas, `parse_range` handles `a-b`, and `parse_port` validates each number with `std::from_chars`.
2. Worker thread `w` scans ports `w, w+50, w+100, ...` and writes results to its own slots of a shared `std::vector`, so no synchronization is needed.
3. Each port gets a fresh socket with a 1 s timeout. `connect` returning `0` means **open**, `ECONNREFUSED` means **closed**, and anything else (a timeout) means **filtered**.
4. For open ports the scanner reads up to 256 bytes. If the service stays silent, it sends `HEAD / HTTP/1.0` and extracts the `Server:` header.

## Project structure

```
include/portscanner/
  ports.hpp      port string parsing
  scanner.hpp    PortState, scan_port, grab_banner
src/
  main.cpp       command line and output
  ports.cpp
  scanner.cpp    RAII Socket class, connect scan, banner grabbing
```

## Known limitations

- **VPN / transparent proxies**: tools like Xray or v2ray in TUN mode accept every connection themselves, so all ports appear open. Scan without them.
- **TCP self-connect**: when scanning localhost, a port in the ephemeral range can occasionally connect to itself and show up as open.
- IPv4 only.

## Roadmap

- [ ] Unit tests and GitHub Actions CI
- [ ] `-t` flag for thread count
- [ ] JSON output
- [ ] SYN scan with raw sockets
- [ ] UDP scan

## Legal

Only scan hosts you own or have explicit permission to test.
