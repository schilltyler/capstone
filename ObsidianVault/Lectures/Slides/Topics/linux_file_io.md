


---
<!-- slide template="[[Split Vertical]]" -->

::: title

#### Linux I/O on AArch64:  Syscalls

:::
::: left
- Focus: file I/O syscalls on AArch64
- I.e. "How do we read  and filter through files"
- Use direct `syscall()` 
- Explore kernel internals & structs
- Build efficient scanner for a 2GB file
- In-class assignment: structured raw file scan
::: 
::: right 

![[man_poertry.png|500]]
:::



---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Finding Syscall values

:::
```
cat /usr/include/asm-generic/unistd.h | grep openat
#define __NR_openat 56
__SYSCALL(__NR_openat, sys_openat)
#define __NR_openat2 437
__SYSCALL(__NR_openat2, sys_openat2)

```

```
grep -ho "__NR_[a-zA-Z0-9_]\+\s\+[0-9]\+" /usr/include/asm-generic/unistd.h | \
  sed 's/__NR_//' | column -t
```

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Aarch64 IO

:::
![[aarch_man_open.png]]

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
int fd = syscall(SYS_openat, AT_FDCWD, "file.txt", O_RDONLY);
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
 ├─ mm → mm_struct
 │    └─ vm_area_struct list/tree
```


---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Kernel Objects

:::
- How to find kernel structs in linux 
- `struct file` — [https://github.com/torvalds/linux/blob/4ff71af020ae59ae2d83b174646fc2ad9fcd4dc4/include/linux/fs.h#L1099](https://github.com/torvalds/linux/blob/4ff71af020ae59ae2d83b174646fc2ad9fcd4dc4/include/linux/fs.h#L1099)
    
- `struct file_operations` [https://github.com/torvalds/linux/blob/4ff71af020ae59ae2d83b174646fc2ad9fcd4dc4/include/linux/fs.h](https://github.com/torvalds/linux/blob/4ff71af020ae59ae2d83b174646fc2ad9fcd4dc4/include/linux/fs.h)
    
- `struct mm_struct` [https://github.com/torvalds/linux/blob/4ff71af020ae59ae2d83b174646fc2ad9fcd4dc4/include/linux/mm_types.h](https://github.com/torvalds/linux/blob/4ff71af020ae59ae2d83b174646fc2ad9fcd4dc4/include/linux/mm_types.h
    
- `struct vm_area_struct` [https://github.com/torvalds/linux/blob/4ff71af020ae59ae2d83b174646fc2ad9fcd4dc4/include/linux/mm_types.h](https://github.com/torvalds/linux/blob/4ff71af020ae59ae2d83b174646fc2ad9fcd4dc4/include/linux/mm_types.h)

---
<!-- slide template="[[Base Slide]]" -->

::: title
#### man `openat`

:::

- Open file relative to directory (or AT_FDCWD for cwd)
	- Or create depending on arguments `O_*`
- On success, kernel creates struct file, updates task_struct->files
	- Inserts into fd table

Userland:

```c
#include <syscall.h>
...
int fd = syscall(SYS_openat, AT_FDCWD, "/tmp/ch0nky.txt", O_RDONLY);
```


---
<!-- slide template="[[Base Slide]]" -->

::: title
#### man `read`

:::

- Reads bytes into user buffer
- Kernel uses page cache, `copy_to_user(...)`
- Updates `file->f_pos`

Userland:
```c
char buf[4096];
ssize_t n = syscall(SYS_read, fd, buf, sizeof(buf));
```


---
<!-- slide template="[[Base Slide]]" -->

::: title
#### man `write`

:::

- Copies data from user → kernel
- Updates page cache, marks pages dirty 
- Advances `file->f_pos`
- Logically used to send data to an object manged by the kernel (file, pipe,..etc)

Userland:
```c
const char *msg = "Hello\n";
syscall(SYS_write, fd, msg, strlen(msg));
// example: writing data to stdout 
syscall(SYS_write, 1, msg, strlen(msg));
```

---
<!-- slide template="[[Base Slide]]" -->

::: title
#### `stat` / `fstat`

:::

- Retrieves file metadata from inode
- No new file object created
- Useful for size, mode, timestamps

Userland:
```c
struct stat st;
syscall(SYS_fstat, fd, &st);
printf("Size: %lld\n", (long long) st.st_size);
```



---
<!-- slide template="[[Base Slide]]" -->

::: title
#### man `lseek`

:::

- Moves file offset (file->f_pos)
- SEEK_SET, SEEK_CUR, SEEK_END
- Only for seekable fds

Userland:
```c

off_t size = syscall(SYS_lseek, fd, 0, SEEK_END);
```


---
<!-- slide template="[[Base Slide]]" -->

::: title
#### man `close`

:::

- Releases fd from files_struct
- Decrements struct file refcount
- May free file object

Userland:
```c

syscall(SYS_close, fd);
```


---
<!-- slide template="[[Base Slide]]" -->

::: title
#### man `pread / pwrite`

:::
- `pread(fd, buf, count, offset)`
- Reads from fd at offset
- Does not change file->f_pos
- Atomic (no race lseek+read)
- pwrite = write at offset
- Syscall: __NR_pread64 (AArch64 = 67)


Userland:
```

char buf[16];
ssize_t n = syscall(SYS_pread64, fd, buf, 16, 100);
```



---
<!-- slide template="[[Base Slide]]" -->

::: title
#### man `mmap`

:::

- Maps file region into process memory
- Creates new vm_area_struct in mm_struct
- Pages loaded lazily on fault

Userland:
```c
char *map = syscall(SYS_mmap, NULL, size,
                    PROT_READ, MAP_PRIVATE, fd, 0);
```

---
<!-- slide template="[[Base Slide]]" -->

::: title
#### man `munmap`

:::

- munmap: removes VMA from mm_struct
- Kernel updates VMA flags + page tables

Userland:

```c
syscall(SYS_munmap, map, size);
```

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### man `mprotect`

:::
-  mprotect: changes page protections
- Kernel updates VMA flags + page tables

Userland:

```c
syscall(SYS_munmap, map, size);
```

---
<!-- slide template="[[Base Slide]]" -->

::: title
#### Efficient Large File Scanning

:::
Goal: find lines starting with "FLAG{" in 2GB file.

Steps:
- openat file
- fstat size
- mmap whole file (or chunked)
- Scan for prefix after newline/start
- munmap + close


---
<!-- slide template="[[Base Slide]]" -->

::: title
#### Example Scanner

:::
```c

char *data = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
for (off_t i = 0; i < size; i++) {
    if (i == 0 || data[i-1] == '\n') {
        if (memcmp(&data[i], "FLAG{", 5) == 0) {
            // Found line
        }
    }
}
munmap(data, size);
```



---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Discussion

:::
- Implant developer's perspective: what  uses of file IO might we need? 