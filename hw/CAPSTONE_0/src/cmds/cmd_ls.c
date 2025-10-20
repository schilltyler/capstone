/* cmd_ls.c - Directory listing command handler */
#include "../../inc/cmds.h"
#include "../../inc/utils.h"
#include "../../inc/syscalls.h"
#include "../../inc/types.h"
#include "../../inc/minc.h"

int handle_ls(const char *path, struct rpc_response *resp) {
#ifdef TODO_CMD_ls
  /* TODO: Implement ls - list directory contents using getdents64 syscall */
  resp->status = STATUS_ERROR;
  strcpy(resp->data, "ls not yet implemented");
  resp->data_len = strlen(resp->data);
  return -1;
#else
    // Your solution here!
    
    int fd = sys_openat(AT_FDCWD, path, O_RDONLY, 0);
    if (fd < 0) {
        resp->status = STATUS_ERROR;
        return 0;
    }
    
    char dirp[1024];
    
    for (;;) {
    
        ssize_t raw_bytes_read = sys_getdents64(fd, dirp, 1024);
    
        if (raw_bytes_read < 0) {
            resp->status = STATUS_ERROR;
            return 0;
        }
        else if (raw_bytes_read == 0) {
            break;
        }
        else {
            size_t bytes_read = raw_bytes_read;
            resp->data[0] = '\0';
            for (size_t bpos = 0; bpos < bytes_read;) {
                struct linux_dirent64 *d = (struct linux_dirent64 *) (dirp + bpos);
                if (d->d_type == DT_REG) {
                    strcat(resp->data, "[F] ");
                }
                else if (d->d_type == DT_DIR) {
                    strcat(resp->data, "[D] ");
                }
                else if (d->d_type == DT_LNK) {
                    strcat(resp->data, "[S] ");
                }
   
                strcat(resp->data, d->d_name);
                strcat(resp->data, "\n");
                bpos += d->d_reclen;
            }
        }
    }
                  
    resp->status = STATUS_OK;
    
    return 0;

#endif
}
