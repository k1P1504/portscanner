# portscanner

[![CI](https://github.com/k1P1504/portscanner/actions/workflows/ci.yml/badge.svg)](https://github.com/k1P1504/portscanner/actions/workflows/ci.yml)

A fast multithreaded TCP port scanner written from scratch in modern C++20.

```
$ portscanner 127.0.0.1 -p 2222,8000-8001
Порт: 2222 | Состояние: Открыт | SSH-2.0-OpenSSH_9.9
Порт: 8000 | Состояние: Открыт | SimpleHTTP/0.6 Python/3.14.7
Порт: 8001 | Состояние: Закрыт
```

## Features

- **Two scan modes**: full TCP connect scan (`-p`) and half-open SYN scan with hand-crafted raw packets (`-sS`)
- **Flexible port syntax**: single ports, ranges and lists: `22,80,8000-8100`
- **Three port states**: open, closed, filtered (a firewall silently drops packets)
- **Connect timeout**: 1 second instead of the default ~2 minutes for filtered ports
- **Parallel scanning**: 50 worker threads, no locks and no data races
- **Service detection**: reads service banners and asks HTTP servers for their `Server` header
- **RAII socket wrapper**: file descriptors are always closed, even on early return
- **Strict input validation**: rejects `abc`, `0`, `65536`, `80abc`, `8005-8000` and invalid IPs
- **Unit tests**: GoogleTest suite, run on every push by GitHub Actions

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

GoogleTest is downloaded automatically during the first configure.

## Tests

```bash
ctest --test-dir build --output-on-failure
```

## Usage

```bash
./build/portscanner <target> -p <ports>          # connect scan
sudo ./build/portscanner <target> -sS <ports>    # SYN scan, needs root
```

Examples:

```bash
./build/portscanner 127.0.0.1 -p 80
./build/portscanner 127.0.0.1 -p 1-1024
./build/portscanner 127.0.0.1 -p 22,80,443,8000-8100
sudo ./build/portscanner 127.0.0.1 -sS 1-1024
```

### Connect scan vs SYN scan

| | Connect scan (`-p`) | SYN scan (`-sS`) |
|---|---|---|
| Packets per open port | 7 (handshake + teardown) | 3 (`SYN`, `SYN-ACK`, kernel `RST`) |
| Target application sees a connection | yes | no, the handshake never completes |
| Service banners | yes | no |
| Privileges | none | root (`CAP_NET_RAW`) |

## How it works

1. `parse_list` splits the port string by commas, `parse_range` handles `a-b`, and `parse_port` validates each number with `std::from_chars`.
2. Worker thread `w` scans ports `w, w+50, w+100, ...` and writes results to its own slots of a shared `std::vector`, so no synchronization is needed.
3. Each port gets a fresh socket with a 1 s timeout. `connect` returning `0` means **open**, `ECONNREFUSED` means **closed**, and anything else (a timeout) means **filtered**.
4. For open ports the scanner reads up to 256 bytes. If the service stays silent, it sends `HEAD / HTTP/1.0` and extracts the `Server:` header.
5. In SYN mode the scanner builds a 20-byte TCP header by hand (source port 54321, `SYN` flag), computes the checksum over the IPv4 pseudo-header and sends it through a raw socket. It then reads raw packets for up to 1 s, skips everything that is not a reply from the target port to port 54321, and maps `SYN+ACK` to **open** and `RST` to **closed**. The kernel answers the `SYN-ACK` with `RST` on its own, so the connection is never established.

## Project structure

```
include/portscanner/
  ports.hpp      port string parsing
  scanner.hpp    PortState, scan_port, grab_banner
  socket.hpp     RAII Socket class (stream or raw)
  syn.hpp        syn_scan_port
src/
  main.cpp       command line, worker threads, output
  ports.cpp
  scanner.cpp    connect scan, banner grabbing
  syn.cpp        TCP header, checksum, raw send and receive
tests/
  test_ports.cpp GoogleTest unit tests
```

## Known limitations

- **VPN / transparent proxies**: tools like Xray or v2ray in TUN mode accept every connection themselves, so all ports appear open. Scan without them.
- **TCP self-connect**: when scanning localhost, a port in the ephemeral range can occasionally connect to itself and show up as open.
- IPv4 only.

## Roadmap

- [x] Unit tests and GitHub Actions CI
- [ ] `-t` flag for thread count
- [ ] JSON output
- [x] SYN scan with raw sockets
- [ ] UDP scan

## Legal

Only scan hosts you own or have explicit permission to test.
