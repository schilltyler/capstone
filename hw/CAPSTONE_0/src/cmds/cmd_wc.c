/* cmd_wc.c - Word count command handler */
#include "cmds.h"
#include "utils.h"
#include "syscalls.h"
#include "types.h"

int handle_wc(const char *args, struct rpc_response *resp) {
#ifdef TODO_CMD_wc
  /* TODO: Implement word count via mmap */
  resp->status = STATUS_ERROR;
  strcpy(resp->data, "wc not yet implemented");
  resp->data_len = strlen(resp->data);
  return -1;
#else
    // Your solution here!
#endif
}
