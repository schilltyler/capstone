
<script> 

    mermaid.mermaidAPI.initialize({
      startOnLoad:false,
      securityLevel: 'loose',
      lazyLoad: false,
    });

    Reveal.initialize();

    const cb = function (event) { mermaid.init(event.currentSlide, '.present>.mermaid'); };
    Reveal.addEventListener('ready', cb);
    Reveal.addEventListener('slidechanged', cb);

</script>

---
<!-- slide template="[[Split Vertical]]" -->

::: title

#### Linux Socket Programming: TCP Fundamentals

:::
::: left
**What we'll cover:**
- What is a socket?
- TCP vs UDP
- Network byte order
- Socket address structures
- Core socket syscalls
- TCP connection lifecycle
- poll() for I/O multiplexing
- Practical examples with direct syscalls

:::
::: right

![[man_poertry.png|500]]


:::




---
<!-- slide template="[[Base Slide]]" -->

::: title

#### What is a Socket?

:::

**Definition:**
- A socket is an endpoint for network communication
- Abstraction over network protocols (TCP, UDP, etc.)
- Treated as a file descriptor by the kernel
- Enables bidirectional data transfer between processes



---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Socket

:::


![[socket_as_fd.md]]



---
<!-- slide template="[[Base Slide]]" -->
::: title
#### sockets interface
:::
![[socket_abstraction_layers.md]]



---
<!-- slide template="[[Split Vertical]]" -->

::: title

#### TCP vs UDP: Protocol Comparison

:::
::: left

**TCP (Transmission Control Protocol):**
- **SOCK_STREAM**
- Connection-oriented
- Reliable, ordered delivery
- Automatic retransmission
- Flow control
- 3-way handshake

Protocols built on TCP:
- HTTP/HTTPS
- SSH
- File transfers
- Database connections
- **Our C2 agent**


:::
::: right

**UDP (User Datagram Protocol):**
- **SOCK_DGRAM**
- Connectionless
- Best-effort delivery/Fire and forget
- No retransmission
- No ordering guarantees
- No handshake

Protocols Built on UDP
- DNS queries
- Video streaming
- Gaming
- VoIP


:::


---
<!-- slide template="[[Base Slide]]" -->

::: title

#### TCP Segment Structure

:::

```
TCP Header (20 bytes minimum):

 0                   1                   2                   3
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|          Source Port          |       Destination Port        |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                        Sequence Number                        |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                    Acknowledgment Number                      |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|  Data |       |C|E|U|A|P|R|S|F|                               |
| Offset| Rsrvd |W|C|R|C|S|S|Y|I|            Window             |
|       |       |R|E|G|K|H|T|N|N|                               |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|           Checksum            |         Urgent Pointer        |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                    Options (variable)                         |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                             Data                              |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
```



---
<!-- slide template="[[Base Slide]]" -->

::: title
#### TCP frame fields

:::

- **Source/Dest Port:** 16-bit port numbers (0-65535)
- **Sequence Number:** Track bytes sent
- **ACK Number:** Acknowledge bytes received
- **Flags:** SYN, ACK, FIN (connection control)
- **Window:** Flow control


---
<!-- slide template="[[Base Slide]]" -->

::: title

#### TCP 3-Way Handshake

:::


![[tcp_3way_handshake.md]]



---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Handshake 

:::
**Why 3 steps?**
1. SYN: Client says "I want to connect"
2. SYN-ACK: Server says "OK, I'm ready"
3. ACK: Client confirms "Got it, let's go"

**Prevents:**
- Old duplicate connections
- Synchronizes sequence numbers


---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Socket Address Structures

:::

**Generic address structure:**
```c
struct sockaddr {
    uint16_t sa_family;    // Address family (AF_INET, AF_INET6, AF_UNIX)
    char     sa_data[14];  // Protocol-specific address data
};
```

**IPv4-specific structure:**
```c
struct sockaddr_in {
    uint16_t      sin_family;  // AF_INET (always 2)
    uint16_t      sin_port;    // Port number (network byte order!)
    struct in_addr sin_addr;   // IPv4 address (network byte order!)
    char          sin_zero[8]; // Padding to match sockaddr size
};

struct in_addr {
    uint32_t s_addr;           // IPv4 address (32 bits)
};
```


---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Example: Filling in an addr struct

:::
```c
struct sockaddr_in addr = {0};
addr.sin_family = AF_INET;                       // IPv4
addr.sin_port = htons("4444");                     // Port 4444 (convert to network order)
addr.sin_addr.s_addr = htonl(0xC0A80164);       // 192.168.1.100 (convert to network order)

// Or using builder approach:
addr.sin_addr.s_addr = htonl((192 << 24) | (168 << 16) | (1 << 8) | 100);
```

**Important:** Always use `sockaddr_in` for IPv4, then cast to `struct sockaddr *` in syscalls


---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Network Byte Order

:::

- Different CPUs store multi-byte values differently
- **Little-endian** (x86, ARM): LSB first → `0x1234` stored as `34 12`
- **Big-endian** (network, old SPARC): MSB first → `0x1234` stored as `12 34`
- Network protocols use **big-endian** (network byte order)


---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Network byte Order 

:::

```
Value: 0x12345678

Little-endian (x86/ARM):          Big-endian (Network):
Memory address: 0x100             Memory address: 0x100
┌─────┬─────┬─────┬─────┐        ┌─────┬─────┬─────┬─────┐
│ 78  │ 56  │ 34  │ 12  │        │ 12  │ 34  │ 56  │ 78  │
└─────┴─────┴─────┴─────┘        └─────┴─────┴─────┴─────┘
 0x100 0x101 0x102 0x103          0x100 0x101 0x102 0x103
```


---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Converting on Aarch64

:::
```c
// Host to Network Short (16-bit)
static uint16_t htons(uint16_t n) {
    return ((n & 0xff) << 8) | ((n & 0xff00) >> 8);
}

// Host to Network Long (32-bit)
static uint32_t htonl(uint32_t n) {
    return ((n & 0xff) << 24) |
           ((n & 0xff00) << 8) |
           ((n & 0xff0000) >> 8) |
           ((n & 0xff000000) >> 24);
}

// Network to Host (symmetric operations)
#define ntohs htons
#define ntohl htonl
```

**Rule:** Convert ALL multi-byte values (port, IP) before sending, and after receiving


---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Socket Syscall Numbers

:::


```bash
cat /usr/include/asm-generic/unistd.h | grep -E 'socket|bind|listen|accept|connect'
#define __NR_socket 198
#define __NR_bind 200
#define __NR_listen 201
#define __NR_accept 202
#define __NR_connect 203
#define __NR_sendto 206
#define __NR_recvfrom 207
#define __NR_shutdown 210
#define __NR_ppoll 73
```

**Define them in your code:**
```c
#define SYS_socket    198
#define SYS_bind      200
#define SYS_listen    201
#define SYS_accept    202
#define SYS_connect   203
#define SYS_sendto    206
#define SYS_recvfrom  207
#define SYS_shutdown  210
#define SYS_ppoll     73
```





---
<!-- slide template="[[Base Slide]]" -->

::: title

####  man  `socket`

:::

**Purpose:** Create a socket endpoint

**Signature:**
```c
int socket(int domain, int type, int protocol);
```

**Parameters:**
- `domain`: Address family
  - `AF_INET` (2) - IPv4
  - `AF_INET6` (10) - IPv6
  - `AF_UNIX` (1) - Local IPC
- `type`: Socket type
  - `SOCK_STREAM` (1) - TCP (reliable, ordered)
  - `SOCK_DGRAM` (2) - UDP (unreliable datagrams)
- `protocol`: Usually 0 (auto-select for type)

**Returns:** File descriptor (≥0) on success, -1 on error




---
<!-- slide template="[[Base Slide]]" -->

::: title

#### `socket` 

:::
**Example:**
```c
// Create TCP socket
int sockfd = syscall3(SYS_socket, AF_INET, SOCK_STREAM, 0);
if (sockfd < 0) {
    // Handle error
}
```

**What happens in the kernel:**
1. Allocate `struct socket` and `struct sock`
2. Initialize protocol-specific data (TCP state machine)
3. Create file descriptor entry
4. Return fd to userspace


---
<!-- slide template="[[Base Slide]]" -->

::: title

#### man `bind`

:::

- Assign an address (IP + port) to a socket

**Signature:**
```c
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```

**Parameters:**
- `sockfd`: Socket file descriptor (from `socket()`)
- `addr`: Pointer to address structure (cast `sockaddr_in *` to `sockaddr *`)
- `addrlen`: Size of address structure (`sizeof(struct sockaddr_in)`)

**Returns:** 0 on success, -1 on error


---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Example (server side) 

:::
```c
struct sockaddr_in addr = {0};
addr.sin_family = AF_INET;
addr.sin_port = htons(4444);               // Listen on port 4444
addr.sin_addr.s_addr = htonl(0x00000000);  // Listen on all interfaces (0.0.0.0)

int ret = syscall3(SYS_bind, sockfd, (long)&addr, sizeof(addr));
if (ret < 0) {
    // Handle error (e.g., port already in use)
}
```

**Common errors:**
- **EADDRINUSE** (98): Port already in use
- **EACCES** (13): Permission denied (ports < 1024 require root)


---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Core Socket Syscalls: listen()

:::

**Purpose:** Mark socket as passive (ready to accept connections)

**Signature:**
```c
int listen(int sockfd, int backlog);
```

**Parameters:**
- `sockfd`: Socket file descriptor (must be bound first)
- `backlog`: Max number of pending connections in queue (typically 5-128)

**Returns:** 0 on success, -1 on error


---
<!-- slide template="[[Base Slide]]" -->

::: title

#### `listen` 

:::
```c
int ret = syscall2(SYS_listen, sockfd, 5);
if (ret < 0) {
    // Handle error
}
```


![[listen_accept_queue.md]]

**Note:**
- Socket is now in **LISTEN** state
- Incoming SYN packets are queued
- `accept()` will dequeue connections


---
<!-- slide template="[[Base Slide]]" -->

::: title

####  man `accept`

:::

- Accept a pending connection from the queue

```c
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
```

**Parameters:**
- `sockfd`: Listening socket fd
- `addr`: Pointer to store client address (can be NULL)
- `addrlen`: Pointer to size (can be NULL)

**Returns:** New connected socket fd on success, -1 on error



---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Example

:::
**Example:**
```c
struct sockaddr_in client_addr;
socklen_t client_len = sizeof(client_addr);

int connfd = syscall3(SYS_accept, sockfd, (long)&client_addr, (long)&client_len);
if (connfd < 0) {
    // Handle error
}

// Now use connfd for send/recv
// Original sockfd still listening for more connections
```

**Important:**
- **Blocks** until a client connects
- Returns a **NEW** file descriptor for this connection
- Original `sockfd` remains in LISTEN state


---
<!-- slide template="[[Base Slide]]" -->

::: title

#### man `connect`

:::

- Initiate connection to a server (client-side)

**Signature:**
```c
int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```

**Parameters:**
- `sockfd`: Socket fd (from `socket()`, no need to bind)
- `addr`: Server address structure
- `addrlen`: Size of address structure

**Returns:** 0 on success, -1 on error

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Client example 

:::
```c
// Create socket
int sockfd = syscall3(SYS_socket, AF_INET, SOCK_STREAM, 0);

// Set server address (192.168.1.100:4444)
struct sockaddr_in server_addr = {0};
server_addr.sin_family = AF_INET;
server_addr.sin_port = htons(4444);
server_addr.sin_addr.s_addr = htonl((192 << 24) | (168 << 16) | (1 << 8) | 100);

// Connect
int ret = syscall3(SYS_connect, sockfd, (long)&server_addr, sizeof(server_addr));
if (ret < 0) {
    // Handle error (ECONNREFUSED, ETIMEDOUT, etc.)
}

// Connection established, ready to send/recv
```

**What happens:** Initiates TCP 3-way handshake (blocks until complete)


---
<!-- slide template="[[Base Slide]]" -->

::: title

#### man `send` man `recv`

:::

**send() / sendto():**
```c
ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,
               const struct sockaddr *dest_addr, socklen_t addrlen);
```

**recv() / recvfrom():**
```c
ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,
                 struct sockaddr *src_addr, socklen_t *addrlen);
```



---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Example 

:::
**For TCP (connected sockets), we can pass NULL for address:**
```c
// Send data
ssize_t sent = syscall6(SYS_sendto, sockfd, (long)buffer, length, 0, 0, 0);
if (sent < 0) {
    // Error
} else if (sent < length) {
    // Partial send, need to send remaining bytes
}

// Receive data
ssize_t received = syscall6(SYS_recvfrom, sockfd, (long)buffer, sizeof(buffer), 0, 0, 0);
if (received < 0) {
    // Error
} else if (received == 0) {
    // Connection closed by peer
}
```

**Important:**
- Both can return **less** than requested (partial I/O)
	- i.e. send() doesn’t mean the data was transmitted across the network yet — only that it was copied into the kernel’s send buffer. TCP itself will handle segmentation (MSS-sized packets),

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Helpers: send_exact() and recv_exact()

:::

-  `send()` and `recv()` can transfer less than requested
	-  Loop until all bytes are transferred

```c
// Send exactly n bytes (block until complete)
static int send_exact(int sockfd, const void *buf, size_t n) {
    const char *p = buf;
    size_t remaining = n;
    while (remaining > 0) {
        ssize_t sent = syscall6(SYS_sendto, sockfd, (long)p, remaining, 0, 0, 0);
        if (sent < 0) {
            return -1;  // Error
        }
        p += sent;
        remaining -= sent;
    }
    return 0;
}
// Receive exactly n bytes (block until complete)
static int recv_exact(int sockfd, void *buf, size_t n) {
    char *p = buf;
    size_t remaining = n;
    while (remaining > 0) {
        ssize_t received = syscall6(SYS_recvfrom, sockfd, (long)p, remaining, 0, 0, 0);
... similar error handling ...
    }
    return 0;
}
```


---
<!-- slide template="[[Base Slide]]" -->

::: title

#### TCP Connection Lifecycle

:::

![[tcp_connection_lifecycle.md]]


---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Complete Example: TCP Server

:::

```c
void tcp_server_example(void) {
    // 1. Create socket
    int sockfd = syscall3(SYS_socket, AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        syscall1(SYS_exit, 1);
    }

    // 2. Bind to address
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(4444);
    addr.sin_addr.s_addr = htonl(0x00000000);  // 0.0.0.0 (all interfaces)

    if (syscall3(SYS_bind, sockfd, (long)&addr, sizeof(addr)) < 0) {
        syscall1(SYS_exit, 1);
    }

    // 3. Listen
    if (syscall2(SYS_listen, sockfd, 5) < 0) {
        syscall1(SYS_exit, 1);
    }

    // 4. Accept connection
    int connfd = syscall3(SYS_accept, sockfd, 0, 0);
    if (connfd < 0) {
        syscall1(SYS_exit, 1);
    }

    // 5. Communicate
    char buffer[256];
    ssize_t n = syscall6(SYS_recvfrom, connfd, (long)buffer, sizeof(buffer), 0, 0, 0);

    const char *response = "Hello from server\n";
    syscall6(SYS_sendto, connfd, (long)response, strlen(response), 0, 0, 0);

    // 6. Close
    syscall1(SYS_close, connfd);
    syscall1(SYS_close, sockfd);
}
```


---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Complete Example: TCP Client

:::

```c
void tcp_client_example(void) {
    // 1. Create socket
    int sockfd = syscall3(SYS_socket, AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        syscall1(SYS_exit, 1);
    }

    // 2. Set server address (127.0.0.1:4444)
    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(4444);
    server_addr.sin_addr.s_addr = htonl((127 << 24) | (0 << 16) | (0 << 8) | 1);

    // 3. Connect
    if (syscall3(SYS_connect, sockfd, (long)&server_addr, sizeof(server_addr)) < 0) {
        syscall1(SYS_exit, 1);
    }

    // 4. Send data
    const char *message = "Hello from client\n";
    ssize_t sent = syscall6(SYS_sendto, sockfd, (long)message, strlen(message), 0, 0, 0);

    // 5. Receive response
    char buffer[256];
    ssize_t received = syscall6(SYS_recvfrom, sockfd, (long)buffer, sizeof(buffer), 0, 0, 0);

    // 6. Close
    syscall1(SYS_close, sockfd);
}
```


---
<!-- slide template="[[Base Slide]]" -->

::: title

#### man `poll`: I/O  Multiplexing

:::

- Wait for data on socket
- Wait for file changes (inotify)
- Timeout if nothing happens
- Allows for monitoring multiple file descriptors simultaneously 
	- See also `epoll` for when you have **a lot** of fds


**Signature:**
```c
int poll(struct pollfd *fds, nfds_t nfds, int timeout);

struct pollfd {
    int   fd;        // File descriptor
    short events;    // Requested events (POLLIN, POLLOUT, ...)
    short revents;   // Returned events (filled by kernel)
};
```

**Returns:** Number of ready fds, 0 on timeout, -1 on error


---
<!-- slide template="[[Base Slide]]" -->

::: title

#### `poll` events 

:::
- `POLLIN` (0x0001) - Data available to read
- `POLLOUT` (0x0004) - Ready for writing
- `POLLERR` (0x0008) - Error condition
- `POLLHUP` (0x0010) - Hang up (connection closed)



---
<!-- slide template="[[Base Slide]]" -->

::: title

#### poll() Example: `inotify`

:::
- man  `inotify` - monitoring filesystem events
- Ex: Wait for data on socket OR file modification event

```c
// Setup inotify for file watching
int inotify_fd = syscall1(SYS_inotify_init1, 0);
int watch_fd = syscall3(SYS_inotify_add_watch, inotify_fd,
 (long)"/var/log/syslog", 0x00000002);
// Setup poll structures
struct pollfd fds[2];
// Watch inotify fd for file changes
fds[0].fd = inotify_fd;
fds[0].events = POLLIN;
fds[0].revents = 0;
// Watch socket for incoming data
fds[1].fd = sockfd;
fds[1].events = POLLIN;
fds[1].revents = 0;
// Poll (block forever, timeout = -1)
// wtf is ppoll? man ppoll :-)
long ret = syscall3(SYS_ppoll, (long)fds, 2, 0);  // 0 = NULL timeout
if (ret > 0) {
    if (fds[0].revents & POLLIN) {
        // File was modified, read new data
    }
    if (fds[1].revents & POLLIN) {
        // Data on socket (maybe a cancel command)
    }
}
```


---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Kernel Socket Structures

:::

![[kernel_socket_structures.md]]


---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Socket in the kernel 

:::
- foo bar
- `struct file` - Generic file descriptor ([kernel source](https://github.com/torvalds/linux/blob/master/include/linux/fs.h))
- `struct socket` - Socket layer ([kernel source](https://github.com/torvalds/linux/blob/master/include/linux/net.h))
- `struct sock` - Protocol-specific data ([kernel source](https://github.com/torvalds/linux/blob/master/include/net/sock.h))


---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Socket Quick Reference

:::

| Syscall               | Purpose             | Server | Client | Returns        |
| --------------------- | ------------------- | :----: | :----: | -------------- |
| `socket()`            | Create socket       |   ✅    |   ✅    | fd             |
| `bind()`              | Assign address      |   ✅    |        | 0/-1           |
| `listen()`            | Mark as passive     |   ✅    |        | 0/-1           |
| `accept()`            | Accept connection   |   ✅    |        | connfd         |
| `connect()`           | Initiate connection |        |   ✅    | 0/-1           |
| `send()`/`sendto()`   | Send data           |   ✅    |   ✅    | bytes sent     |
| `recv()`/`recvfrom()` | Receive data        |   ✅    |   ✅    | bytes received |
| `poll()`              | Wait for events     |   ✅    |   ✅    | ready count    |
| `shutdown()`          | Close half-duplex   |   ✅    |   ✅    | 0/-1           |
| `close()`             | Close socket        |   ✅    |   ✅    | 0/-1           |

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Socket API in action 

:::

**Server:**
```
socket() → bind() → listen() → accept() → recv()/send() → close()
```

**Client:**
```
socket() → connect() → send()/recv() → close()
```


---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Common Socket Errors

:::

| Error Code   | Name | Meaning                | Common Causes                         |
| ------------ | ---- | ---------------------- | ------------------------------------- |
| EADDRINUSE   | 98   | Address already in use | Port already bound by another process |
| ECONNREFUSED | 111  | Connection refused     | No server listening on target port    |
| ETIMEDOUT    | 110  | Connection timed out   | Network unreachable or server down    |
| EACCES       | 13   | Permission denied      | Binding to port < 1024 without root   |
| EINPROGRESS  | 115  | Operation in progress  | Non-blocking connect still completing |
| EPIPE        | 32   | Broken pipe            | Sending to closed connection          |
| ECONNRESET   | 104  | Connection reset       | Peer closed connection abruptly       |


---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Checking Socket Errors 

:::
```c
int ret = syscall3(SYS_connect, sockfd, (long)&addr, sizeof(addr));
if (ret < 0) {
    long err = -ret;  // Negative errno
    if (err == 111) {
        // ECONNREFUSED: No server listening
    } else if (err == 110) {
        // ETIMEDOUT: Network unreachable
    }
}
```





---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Further Reading

:::

**Man Pages:**
- `man 2 socket` - socket() syscall
- `man 2 bind` - bind() syscall
- `man 2 connect` - connect() syscall
- `man 7 tcp` - TCP protocol overview
- `man 7 ip` - IP protocol overview

**Tools:**
- `strace` - Trace syscalls
- `tcpdump` - Capture network traffic
- `wireshark` - Analyze packets
- `netstat` / `ss` - View socket states

DEMO