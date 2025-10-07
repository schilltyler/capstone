/* agent.c - Main entry point and command dispatcher */
#include "cmds.h"
#include "config.h"
#include "rpc.h"
#include "syscalls.h"
#include "types.h"
#include "utils.h"

/* ========================================================================
 * Main function
 * ======================================================================== */

static struct rpc_request g_req;
static struct rpc_response g_resp;

void _start(void) {
  // Your solution here!
  // connect to the serer
  // handle authentication
  // event loop: get RPC Request and execute
}
