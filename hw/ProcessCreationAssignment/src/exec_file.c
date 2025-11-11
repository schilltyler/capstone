#include "runtime.h"
#include "syscalls.h"

#define BUFFER_SIZE 1024
static const char OUTPUT_PATH[] = "/tmp/process_creation_output.txt";

static void print_usage(const char *progname) {
    mini_printf("Usage: %s <executable> [args...]\n", progname);
}

static void stream_file(int fd) {
    char buffer[BUFFER_SIZE];
    while (1) {
        long nread = sys_read(fd, buffer, BUFFER_SIZE - 1);
        if (nread < 0) {
            die("read failed");
        }
        if (nread == 0) {
            break;
        }
        buffer[nread] = '\0';
        mini_printf("Child Output: %s", buffer);
    }
}

int main(int argc, char **argv, char **envp) {
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }

    // Your solution here!

    return 0;
}
