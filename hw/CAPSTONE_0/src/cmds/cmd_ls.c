/* cmd_ls.c - Directory listing command handler */
#include "cmds.h"
#include "utils.h"
#include "syscalls.h"
#include "types.h"

int handle_ls(const char *path, struct rpc_response *resp) {
#ifdef TODO_CMD_ls
  /* TODO: Implement ls - list directory contents using getdents64 syscall */
  resp->status = STATUS_ERROR;
  strcpy(resp->data, "ls not yet implemented");
  resp->data_len = strlen(resp->data);
  return -1;
#else
    // Your solution here!
#endif
}
