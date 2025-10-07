/* cmd_djb2sum.c - DJB2 hash calculation handler */
#include "cmds.h"
#include "syscalls.h"
#include "types.h"
#include "utils.h"

int handle_djb2sum(const char *path, struct rpc_response *resp) {
#ifdef TODO_CMD_djb2sum
  /* TODO: Implement djb2 hash calculation - efficiently hash file using mmap */
  resp->status = STATUS_ERROR;
  strcpy(resp->data, "djb2sum not yet implemented");
  resp->data_len = strlen(resp->data);
  return -1;
#else
    // Your solution here!
#endif
}
