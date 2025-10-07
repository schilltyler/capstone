/* cmd_timestomp.c - File timestamp modification handler */
#include "cmds.h"
#include "syscalls.h"
#include "types.h"
#include "utils.h"

int handle_timestomp(const char *args, struct rpc_response *resp) {
#ifdef TODO_CMD_timestomp
  /* TODO: Implement timestamp modification */
  resp->status = STATUS_ERROR;
  strcpy(resp->data, "timestomp not yet implemented");
  resp->data_len = strlen(resp->data);
  return -1;
#else
    // Your solution here!
#endif
}
