
```mermaid
graph LR
    A[Client 1 SYN] --> B[Accept Queue]
    C[Client 2 SYN] --> B
    D[Client 3 SYN] --> B
    B --> E[accept syscall]

    style B fill:#4ecdc4
```
