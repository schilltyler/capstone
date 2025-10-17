/* cmd_stats.c - File statistics command handler */
#include "cmds.h"
#include "utils.h"
#include "syscalls.h"
#include "types.h"

int handle_get_file_stats(const char *path, struct rpc_response *resp) {
#ifdef TODO_CMD_stats
  /* TODO: Implement stats - get file statistics using fstatat syscall */
  resp->status = STATUS_ERROR;
  strcpy(resp->data, "stats not yet implemented");
  resp->data_len = strlen(resp->data);
  return -1;
#else
    // Your solution here!

    // 130 is a couple of bytes more than what the stat
    // struct is (128)
    struct stat buf[130];

    int result = sys_newfstatat(AT_FDCWD, path, buf, 0);
    if (result == -1) {
        resp->status = STATUS_ERROR;
        return 0;
    }

    strcat(resp->data, "Size: ");
    // TODO
    
    return 0;
#endif
}
