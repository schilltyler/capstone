/* cmd_bgrep.c - Binary grep command handler */
#include "cmds.h"
#include "syscalls.h"
#include "types.h"
#include "utils.h"

#define CHUNK_SIZE 4096
#define MAX_PATTERN_SIZE 256

int handle_bgrep(int sockfd, const char *path, const char *hex_pattern) {
#ifdef TODO_CMD_bgrep
  /* TODO: Implement binary grep with hex pattern search */
  struct rpc_response resp;
  memset(&resp, 0, sizeof(resp));
  resp.status = STATUS_ERROR;
  strcpy(resp.data, "bgrep not yet implemented");
  resp.data_len = strlen(resp.data);
  send_response(sockfd, &resp);
  return -1;
#else
    // Your solution here!
#endif
}
