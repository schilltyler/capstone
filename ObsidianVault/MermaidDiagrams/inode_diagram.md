
```mermaid
graph TD
    D[Directory Entry: file1] --> I[(Inode #1234)]
    I -->|Metadata| M[Permissions, Owner, Size, Times]
    I -->|Block Pointers| B[Data Blocks on Disk]
```

