#include "runtime.h"
#include "syscalls.h"
#include <stdarg.h>

void *memcpy(void *dest, const void *src, size_t n) {
    unsigned char *d = (unsigned char *)dest;
    const unsigned char *s = (const unsigned char *)src;
    for (size_t i = 0; i < n; i++) {
        d[i] = s[i];
    }
    return dest;
}

void *memset(void *s, int c, size_t n) {
    unsigned char *p = (unsigned char *)s;
    for (size_t i = 0; i < n; i++) {
        p[i] = (unsigned char)c;
    }
    return s;
}

size_t strlen(const char *s) {
    size_t len = 0;
    if (!s) {
        return 0;
    }
    while (s[len] != '\0') {
        len++;
    }
    return len;
}

void write_str(int fd, const char *s) {
    if (!s) {
        return;
    }
    sys_write(fd, s, strlen(s));
}

void write_char(int fd, char c) {
    sys_write(fd, &c, 1);
}

static void write_uint(unsigned long value) {
    char buf[32];
    int i = 0;

    if (value == 0) {
        write_char(STDOUT_FILENO, '0');
        return;
    }

    while (value > 0) {
        buf[i++] = '0' + (value % 10);
        value /= 10;
    }

    while (i > 0) {
        write_char(STDOUT_FILENO, buf[--i]);
    }
}

static void write_int(long value) {
    if (value < 0) {
        write_char(STDOUT_FILENO, '-');
        value = -value;
    }
    write_uint((unsigned long)value);
}

static void write_hex(unsigned long value, int prefix) {
    char buf[32];
    int i = 0;

    if (prefix) {
        write_str(STDOUT_FILENO, "0x");
    }

    if (value == 0) {
        write_char(STDOUT_FILENO, '0');
        return;
    }

    while (value > 0) {
        int digit = value & 0xF;
        buf[i++] = (digit < 10) ? ('0' + digit) : ('a' + digit - 10);
        value >>= 4;
    }

    while (i > 0) {
        write_char(STDOUT_FILENO, buf[--i]);
    }
}

void mini_printf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    for (const char *p = fmt; *p != '\0'; p++) {
        if (*p != '%') {
            write_char(STDOUT_FILENO, *p);
            continue;
        }

        p++;
        switch (*p) {
            case 's': {
                const char *s = va_arg(args, const char *);
                write_str(STDOUT_FILENO, s ? s : "(null)");
                break;
            }
            case 'd': {
                int value = va_arg(args, int);
                write_int(value);
                break;
            }
            case 'x': {
                unsigned long value = va_arg(args, unsigned long);
                write_hex(value, 0);
                break;
            }
            case 'p': {
                void *ptr = va_arg(args, void *);
                write_hex((unsigned long)ptr, 1);
                break;
            }
            case '%': {
                write_char(STDOUT_FILENO, '%');
                break;
            }
            default: {
                write_char(STDOUT_FILENO, '%');
                write_char(STDOUT_FILENO, *p);
                break;
            }
        }
    }

    va_end(args);
}

void die(const char *msg) {
    if (msg) {
        write_str(STDERR_FILENO, "fatal: ");
        write_str(STDERR_FILENO, msg);
        write_char(STDERR_FILENO, '\n');
    }
    sys_exit(1);
}

void die_with_status(const char *msg, int status) {
    if (msg) {
        write_str(STDERR_FILENO, "fatal: ");
        write_str(STDERR_FILENO, msg);
        write_char(STDERR_FILENO, '\n');
    }
    sys_exit(status);
}
