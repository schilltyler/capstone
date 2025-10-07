/* cmd_proc_maps.c - Process memory maps handler */
#include "cmds.h"
#include "utils.h"
#include "syscalls.h"
#include "types.h"

int handle_proc_maps(struct rpc_response *resp) {
#ifdef TODO_CMD_proc_maps
  /* TODO: Implement proc_maps - display process memory map */
  resp->status = STATUS_ERROR;
  strcpy(resp->data, "proc_maps not yet implemented");
  resp->data_len = strlen(resp->data);
  return -1;
#else
    // Your solution here!
#endif
}
