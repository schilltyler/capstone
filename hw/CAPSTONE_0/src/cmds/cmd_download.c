/* cmd_download.c - File download command handler */
#include "cmds.h"
#include "utils.h"
#include "syscalls.h"
#include "types.h"

int handle_download_file(int sockfd, const char *path) {
#ifdef TODO_CMD_download
  /* TODO: Implement download - send file to server using multi-chunk protocol */
  struct rpc_response resp;
  memset(&resp, 0, sizeof(resp));
  resp.status = STATUS_ERROR;
  strcpy(resp.data, "download not yet implemented");
  resp.data_len = strlen(resp.data);
  send_response(sockfd, &resp);
  return -1;
#else
    // Your solution here!
#endif
}
