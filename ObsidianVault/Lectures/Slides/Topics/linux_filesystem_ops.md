

---
<!-- slide template="[[Split Vertical]]" -->

::: title

#### Linux Filesystem Operations 

:::
::: left
- Focus: filesystem syscalls on AArch64
- I.e. "How do we list, traverse, and manage files"
- Explore kernel internals & structs
- Understand Unix permission model
- Build directory walker and file protector
- In-class assignment: recursive file scanner
:::
::: right

![[man_poertry.png|500]]
:::



---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Reminder: Finding Syscall values

:::
```bash
cat /usr/include/asm-generic/unistd.h | grep getdents
#define __NR3_getdents64 61
__SYSCALL(__NR3_getdents64, sys_getdents64)
```

```bash
cat /usr/include/asm-generic/unistd.h | grep faccessat
#define __NR_faccessat 48
__SYSCALL(__NR_faccessat, sys_faccessat)
#define __NR_faccessat2 439
__SYSCALL(__NR_faccessat2, sys_faccessat2)
```

```bash
grep -ho "__NR_[a-zA-Z0-9_]\+\s\+[0-9]\+" /usr/include/asm-generic/unistd.h | \
  sed 's/__NR_//' | column -t
```

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Syscall Interface (AArch64)

:::
- Syscalls invoked with `svc #0`
- Registers:
  - `x8` = syscall number
  - `x0–x5` = up to 6 args
  - return value in `x0`
- Example:

```c
int fd = syscall(SYS_openat, AT_FDCWD, "/tmp", O_RDONLY | O_DIRECTORY);
```

---
<!-- slide template="[[Base Slide]]" -->

::: title
#### Kernel Objects Overview

:::
```
task_struct
 ├─ files → files_struct
 │    └─ fd table → struct file *
 ├─ fs → fs_struct
 │    ├─ pwd (cwd dentry)
 │    └─ root (root dentry)
 └─ mm → mm_struct
```


---
<!-- slide template="[[Split Vertical]]" -->

::: title
#### Inode
:::
::: left 
**Definition:**
- An **inode** (index node) is a data structure on a filesystem that stores metadata about a file.  
- Every file/directory has an inode (except special pseudo-filesystems like procfs).


:::
::: right 
![[inode.excalidraw.svg|900]]

:::




---
<!-- slide template="[[Base Slide]]" -->

::: title

####  Inode continued


:::
- File type (regular, dir, symlink, etc.)
- Permissions and ownership (UID, GID)
- File size
- Timestamps (created, modified, accessed, changed)
- Link count (number of directory entries pointing to it)
- Pointers to data blocks on disk

**Not stored in an inode:**
- **Filename** (stored in the directory entry  (dirent) instead!)

---
<!-- slide template="[[Base Slide]]" -->

::: title
#### Linux File Types
:::

**File types shown by `ls -l` first character:**
- `-` Regular file: standard data file (text, binary, executable, etc.)
- `d` Directory: contains file entries (like a folder)
- `l` Symbolic link: pointer to another file/directory (can cross filesystems)
- `b` Block device: buffered device (e.g., disk)
- `c` Character device: unbuffered device (e.g., terminal, serial port)
- `p` Named pipe (FIFO): interprocess communication
- `s` Socket: endpoint for IPC/network communication

**Links:**
- **Hard link:** another directory entry for the same inode.  
  - Same inode number, data shared.  
  - Cannot span filesystems or link to directories.  
- **Symbolic link (symlink):** special file that points to a pathname.  
  - Can cross filesystems, can point to directories, can dangle if target removed.


---
<!-- slide template="[[Base Slide]]" -->

::: title
#### Hard Link vs. Symbolic Link
:::

**Conceptual model:**
- Hard link → multiple directory entries (filenames) pointing to the **same inode**.  
- Symbolic link → a separate inode that stores the **path** to another file.
![[example_fs.excalidraw.svg]]


---
<!-- slide template="[[Base Slide]]" -->

::: title
#### Unix Permission Model

:::

**File mode bits (16 bits total):** (see with `ls -la`)
```
    15  14  13  12 | 11  10   9 |  8   7   6 |  5   4   3 |  2   1   0
[file type bits]  |  [special]  | [user rwx] | [group rwx]| [other rwx]
                  | suid sgid T |   r  w  x  |  r  w  x   |  r  w  x
```

**Common octal modes:**
- `0755` = rwxr-xr-x (user: rwx, group: rx, other: rx)
- `0644` = rw-r--r-- (user: rw, group: r, other: r)
- `0700` = rwx------ (user: rwx, group: none, other: none)
- `0600` = rw------- (user: rw, group: none, other: none)

**Special bits:**
- `04000` = setuid  runs as file owner ( user id)
- `02000` = setgid runs as file group (group id)
- `01000` = sticky bit only owner can delete

---
<!-- slide template="[[Base Slide]]" -->

::: title
#### man `fstatat` / `newfstatat`

:::

- Get file metadata without opening file
- Use AT_FDCWD for relative to cwd
- AT_SYMLINK_NOFOLLOW flag to not follow symlinks
- Returns struct stat with inode metadata

Userland:
```c
#include <sys/stat.h>
#include <fcntl.h>

struct stat st;
int ret = syscall(SYS_newfstatat, AT_FDCWD, "/etc/passwd", &st, 0);
if (ret == 0) {
    printf("Size: %ld\n", st.st_size);
    printf("Mode: %o\n", st.st_mode & 0777);
    printf("UID: %d, GID: %d\n", st.st_uid, st.st_gid);
}
```
---
<!-- slide template="[[Base Slide]]" -->

::: title
#### man `fchmodat`

:::

- Changes file permission bits
- Use AT_FDCWD for relative to cwd
- Does not follow symlinks by default on most implementations
- Updates inode->i_mode

Userland:
```c
#include <sys/stat.h>
#include <fcntl.h>

// Make file readable/writable only by owner
int ret = syscall(SYS_fchmodat, AT_FDCWD, "/tmp/secret.txt", 0600);

// Make directory accessible only by owner
ret = syscall(SYS_fchmodat, AT_FDCWD, "/home/user/.ssh", 0700);
```


---
<!-- slide template="[[Base Slide]]" -->

::: title
#### man `fchownat`

:::

- Changes file ownership (UID/GID)
- Requires CAP_CHOWN capability
- AT_SYMLINK_NOFOLLOW to not follow symlinks
- Updates inode owner/group

Userland:
```c
#include <unistd.h>
#include <fcntl.h>

// Change owner to UID 1000, GID 1000
int ret = syscall(SYS_fchownat, AT_FDCWD, "/tmp/file.txt",
                  1000, 1000, 0);

// Change owner of symlink itself
ret = syscall(SYS_fchownat, AT_FDCWD, "/tmp/link",
              1000, 1000, AT_SYMLINK_NOFOLLOW);
```



---
<!-- slide template="[[Base Slide]]" -->

::: title
#### man `getdents64`

:::

- Reads directory entries into buffer
- Returns variable-length `struct linux_dirent64`
- Kernel iterates through dentry cache
- Much more efficient than readdir() loop

Userland:
```c
#include <syscall.h>
#include <dirent.h>

struct linux_dirent64 {
    ino64_t        d_ino;    // Inode number
    off64_t        d_off;    // Offset to next dirent
    unsigned short d_reclen; // Length of this dirent
    unsigned char  d_type;   // File type
    char           d_name[]; // Filename (null-terminated)
};

char buf[4096];
int fd = syscall(SYS_openat, AT_FDCWD, "/tmp", O_RDONLY | O_DIRECTORY);
long nread = syscall(SYS_getdents64, fd, buf, sizeof(buf));
```





---
<!-- slide template="[[Base Slide]]" -->

::: title
#### man `mkdirat`

:::

- Creates new directory
- Specify mode (permissions)
- AT_FDCWD for relative to cwd
- Fails if directory exists

Userland:
```c
#include <sys/stat.h>
#include <fcntl.h>

// Create directory with mode 0755
int ret = syscall(SYS_mkdirat, AT_FDCWD, "/tmp/mydir", 0755);
if (ret < 0) {
    perror("mkdirat");
}
```


---
<!-- slide template="[[Base Slide]]" -->

::: title
#### man `unlinkat`

:::

- Removes file or empty directory
- AT_REMOVEDIR flag required for directories
- Decrements inode link count
- File deleted when link count reaches 0 and no open fds

Userland:
```c
#include <fcntl.h>
#include <unistd.h>

// Remove file
syscall(SYS_unlinkat, AT_FDCWD, "/tmp/file.txt", 0);

// Remove directory
syscall(SYS_unlinkat, AT_FDCWD, "/tmp/mydir", AT_REMOVEDIR);
```


---
<!-- slide template="[[Base Slide]]" -->

::: title
#### man `getcwd`

:::

- Gets current working directory
- Kernel traverses dentry path to root
- Returns absolute path

Userland:
```c
#include <unistd.h>

char buf[PATH_MAX];
long ret = syscall(SYS_getcwd, buf, sizeof(buf));
if (ret > 0) {
    printf("CWD: %s\n", buf);
}
```


---
<!-- slide template="[[Split Vertical]]" -->

::: title

#### Practical Example: Protecting SSH Keys

:::
::: left

**Goal:** Create .ssh directory and protect private key

**Steps:**
1. Create `.ssh` with mode 0700
2. Create private key file
3. Set private key to 0600
4. Create public key with 0644
5. Verify with fstatat


:::
::: right
**Why?**
- SSH refuses to use keys with wrong permissions
- Prevents other users from reading private keys
- Common operational security practice
:::


---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Code 

:::

```c
#include <stdio.h>
#include <syscall.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define SSH_DIR ".ssh"
#define PRIV_KEY ".ssh/id_rsa"
#define PUB_KEY ".ssh/id_rsa.pub"

int protect_ssh_keys() {
    struct stat st;
    int ret;

    // Create .ssh directory with 0700
    ret = syscall(SYS_mkdirat, AT_FDCWD,
                  SSH_DIR, 0700);
    if (ret < 0 && errno != EEXIST) {
        perror("mkdirat");
        return -1;
    }
```

---
<!-- slide template="[[Base Slide]]" -->

::: title
#### SSH Key Protection (cont.)

:::

```c
    // Verify directory permissions
    ret = syscall(SYS_newfstatat, AT_FDCWD, SSH_DIR, &st, 0);
    if (ret < 0) {
        perror("fstatat");
        return -1;
    }
    // Sanity check: directory should be 0700
    if ((st.st_mode & 0777) != 0700) {
        printf("Warning: %s has mode %o, fixing...\n",
               SSH_DIR, st.st_mode & 0777);
        syscall(SYS_fchmodat, AT_FDCWD, SSH_DIR, 0700);
    }
    // Create private key (or fix existing)
    int fd = syscall(SYS_openat, AT_FDCWD, PRIV_KEY,
                     O_CREAT | O_WRONLY | O_TRUNC, 0600);
    if (fd < 0) {
        perror("openat");
        return -1;
    }
    // Write dummy key data
    const char *key_data = "-----BEGIN PRIVATE KEY-----\n...\n";
    syscall(SYS_write, fd, key_data, strlen(key_data));
    syscall(SYS_close, fd);
```


---
<!-- slide template="[[Base Slide]]" -->

::: title
#### SSH Key Protection (cont.)

:::

```c
    // Verify private key permissions
    ret = syscall(SYS_newfstatat, AT_FDCWD, PRIV_KEY, &st, 0);
    if (ret < 0) {
        perror("fstatat");
        return -1;
    }
    // Sanity check: private key must be 0600
    if ((st.st_mode & 0777) != 0600) {
        printf("ERROR: %s has mode %o (expected 0600)\n",
               PRIV_KEY, st.st_mode & 0777);
        syscall(SYS_fchmodat, AT_FDCWD, PRIV_KEY, 0600);
    }
    // Create public key with 0644 (world-readable)
    fd = syscall(SYS_openat, AT_FDCWD, PUB_KEY,
                 O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd < 0) {
        perror("openat");
        return -1;
    }
    const char *pub_data = "ssh-rsa AAAAB3Nza...\n";
    syscall(SYS_write, fd, pub_data, strlen(pub_data));
    syscall(SYS_close, fd);
    printf("SSH keys protected successfully!\n");
    return 0;
}
```


---
<!-- slide template="[[Base Slide]]" -->

::: title
#### Directory Traversal with getdents64

:::

**Goal:** Walk directory tree recursively

```c
#include <stdio.h>
#include <syscall.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>

void list_directory(const char *path, int depth) {
    char buf[4096];
    int fd, nread, bpos;
    struct linux_dirent64 *d;

    // Open directory
    fd = syscall(SYS_openat, AT_FDCWD, path,
                 O_RDONLY | O_DIRECTORY);
    if (fd < 0) {
        perror("openat");
        return;
    }

    // Read directory entries
    while ((nread = syscall(SYS_getdents64, fd, buf, sizeof(buf))) > 0) {
        for (bpos = 0; bpos < nread;) {
            d = (struct linux_dirent64 *)(buf + bpos);
            ...
```


---
<!-- slide template="[[Base Slide]]" -->

::: title
#### Directory Traversal (cont.)

:::

```c
            // Skip . and ..
            if (strcmp(d->d_name, ".") == 0 ||strcmp(d->d_name, "..") == 0) {
                bpos += d->d_reclen;
                continue;
            }
            // Print with indentation
            for (int i = 0; i < depth; i++) printf("  ");
            // Print type indicator
            char type = '?';
            if (d->d_type == DT_REG) type = 'F';
            else if (d->d_type == DT_DIR) type = 'D';
            else if (d->d_type == DT_LNK) type = 'L';
            printf("[%c] %s\n", type, d->d_name);
            // Recurse into subdirectories
            if (d->d_type == DT_DIR) {
                char subpath[4096];
                snprintf(subpath, sizeof(subpath),
                        "%s/%s", path, d->d_name);
                list_directory(subpath, depth + 1);
            }
            bpos += d->d_reclen;
        }
    }
    syscall(SYS_close, fd);
}
```


---
<!-- slide template="[[Base Slide]]" -->

::: title
#### Sanity Checks for Filesystem Operations

:::


1. **Path lengths:** Check against PATH_MAX (4096)
```c
if (strlen(path) >= PATH_MAX) {
    fprintf(stderr, "Path too long\n");
    return -1;
}
```

2. **NULL pointers:** Validate all pointer arguments
```c
if (!path || !buf) return -EINVAL;
```

3. **Syscall return values:** Always check for errors
```c
if (fd < 0) {
    perror("syscall failed");
    return -1;
}
```

4. **Buffer bounds:** Ensure sufficient space for getdents64
```c
if (d->d_reclen > sizeof(buf) - bpos) break;
```


---
<!-- slide template="[[Base Slide]]" -->

::: title
#### File Type Detection with d_type

:::

From `getdents64`, `d_type` field provides quick type check:

```c
#define DT_UNKNOWN  0   // Unknown type
#define DT_FIFO     1   // Named pipe (FIFO)
#define DT_CHR      2   // Character device
#define DT_DIR      4   // Directory
#define DT_BLK      6   // Block device
#define DT_REG      8   // Regular file
#define DT_LNK      10  // Symbolic link
#define DT_SOCK     12  // Unix domain socket

// Example: filter for regular files only
if (d->d_type == DT_REG) {
    printf("Regular file: %s\n", d->d_name);
}
```

**Note:** Some filesystems return DT_UNKNOWN; use fstatat as fallback.


---
<!-- slide template="[[Base Slide]]" -->

::: title
#### Complete Directory Walker Example

:::

```c
//... headers as above
int walk_directory(const char *path, int max_depth, int current_depth) {
    char buf[8192];
    int fd, nread, bpos;
    struct linux_dirent64 *d;
    // Sanity check: path length
    if (strlen(path) >= PATH_MAX) {
        fprintf(stderr, "Path too long: %s\n", path);
        return -1;
    }
    // Sanity check: recursion depth
    if (current_depth > max_depth) {
        return 0;
    }

    fd = syscall(SYS_openat, AT_FDCWD, path, O_RDONLY | O_DIRECTORY);
    if (fd < 0) {
        if (errno == EACCES) {
            fprintf(stderr, "Permission denied: %s\n", path);
            return 0;
        }
        perror("openat");
        return -1;
    }
```


---
<!-- slide template="[[Base Slide]]" -->

::: title
#### Complete Directory Walker (cont.)

:::

```c
    while ((nread = syscall(SYS_getdents64, fd, buf, sizeof(buf))) > 0) {
        for (bpos = 0; bpos < nread;) {
            d = (struct linux_dirent64 *)(buf + bpos);
            if (strcmp(d->d_name, ".") != 0 && strcmp(d->d_name, "..") != 0) {
                for (int i = 0; i < current_depth; i++) printf("  ");
                printf("%s", d->d_name);
                if (d->d_type == DT_DIR) {
                    printf("/\n");
                    char subpath[PATH_MAX];
                    int len = snprintf(subpath, sizeof(subpath),
                                      "%s/%s", path, d->d_name);
                    // Sanity check: path construction
                    if (len >= PATH_MAX) {
                        fprintf(stderr, "Path too long\n");
                    } else {
                        walk_directory(subpath, max_depth, current_depth + 1);
                    }
                } else { printf("\n"); }
            }
            bpos += d->d_reclen;
        }
    }
    syscall(SYS_close, fd);
    return 0;
}
```


---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Discussion: 

:::
- **Reconnaissance:** How might malware enumerate the filesystem?
  - Looking for SSH keys (`~/.ssh/`)
  - Finding configuration files (`/etc/`, `~/.config/`)
  - Discovering user documents
- **Privilege escalation:** How do permission bits matter?
  - SUID binaries (mode & 04000)
  - World-writable directories
  - Misconfigured sensitive files
- **Persistence:** Where do attackers hide?
  - Hidden directories (names starting with `.`)
  - Unusual permissions that prevent inspection
  - Disguising as system directories
- **Detection:** What syscalls should we monitor?
  - Unexpected `getdents64` on sensitive directories
  - `fchmodat` changing security-critical files
  - Mass file access patterns
