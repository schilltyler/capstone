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
#endif
}
