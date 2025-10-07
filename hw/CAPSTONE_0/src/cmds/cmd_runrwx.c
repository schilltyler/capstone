/* cmd_runrwx.c - Execute raw machine code handler */
#include "cmds.h"
#include "syscalls.h"
#include "types.h"
#include "utils.h"

int handle_runrwx(int sockfd, const char *path) {
#ifdef TODO_CMD_runrwx
  /* TODO: Implement runrwx - execute raw machine code */
  struct rpc_response resp;
  memset(&resp, 0, sizeof(resp));
  resp.status = STATUS_ERROR;
  strcpy(resp.data, "runrwx not yet implemented");
  resp.data_len = strlen(resp.data);
  send_response(sockfd, &resp);
  return -1;
#else
    // Your solution here!
#endif
}
