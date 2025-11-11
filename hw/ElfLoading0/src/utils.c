#include "utils.h"
#include "syscalls.h"
#include <stdarg.h>

// Memory copy - byte by byte
void *memcpy(void *dest, const void *src, size_t n) {
    unsigned char *d = (unsigned char *)dest;
    const unsigned char *s = (const unsigned char *)src;
    for (size_t i = 0; i < n; i++) {
        d[i] = s[i];
    }
    return dest;
}

// Memory set - fill memory with a constant byte
void *memset(void *s, int c, size_t n) {
    unsigned char *p = (unsigned char *)s;
    for (size_t i = 0; i < n; i++) {
        p[i] = (unsigned char)c;
    }
    return s;
}

// String copy - null-terminated
char *strcpy(char *dest, const char *src) {
    char *d = dest;
    while ((*d++ = *src++) != '\0')
        ;
    return dest;
}

// String length
size_t strlen(const char *s) {
    size_t len = 0;
    while (s[len] != '\0') {
        len++;
    }
    return len;
}

// Helper: write string to stdout
static void write_str(const char *s) {
    sys_write(1, s, strlen(s));
}

// Helper: write single character to stdout
static void write_char(char c) {
    sys_write(1, &c, 1);
}

// Helper: convert unsigned integer to decimal string and write
static void write_uint(unsigned long n) {
    char buf[32];
    int i = 0;

    if (n == 0) {
        write_char('0');
        return;
    }

    while (n > 0) {
        buf[i++] = '0' + (n % 10);
        n /= 10;
    }

    // Write in reverse order
    while (i > 0) {
        write_char(buf[--i]);
    }
}

// Helper: convert signed integer to decimal string and write
static void write_int(long n) {
    if (n < 0) {
        write_char('-');
        n = -n;
    }
    write_uint((unsigned long)n);
}

// Helper: convert unsigned integer to hex string and write
static void write_hex(unsigned long n, int print_prefix) {
    char buf[32];
    int i = 0;

    if (print_prefix) {
        write_str("0x");
    }

    if (n == 0) {
        write_char('0');
        return;
    }

    while (n > 0) {
        int digit = n % 16;
        buf[i++] = (digit < 10) ? ('0' + digit) : ('a' + digit - 10);
        n /= 16;
    }

    // Write in reverse order
    while (i > 0) {
        write_char(buf[--i]);
    }
}

// Mini printf implementation
// Supports: %s (string), %d (decimal), %x (hex), %p (pointer), %% (literal %)
void mini_printf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    for (const char *p = fmt; *p != '\0'; p++) {
        if (*p == '%') {
            p++;
            switch (*p) {
                case 's': {
                    const char *s = va_arg(args, const char *);
                    write_str(s ? s : "(null)");
                    break;
                }
                case 'd': {
                    int n = va_arg(args, int);
                    write_int(n);
                    break;
                }
                case 'x': {
                    unsigned int n = va_arg(args, unsigned int);
                    write_hex(n, 0);
                    break;
                }
                case 'p': {
                    void *p = va_arg(args, void *);
                    write_hex((unsigned long)p, 1);
                    break;
                }
                case '%': {
                    write_char('%');
                    break;
                }
                default: {
                    // Unknown format, just print it
                    write_char('%');
                    write_char(*p);
                    break;
                }
            }
        } else {
            write_char(*p);
        }
    }

    va_end(args);
}

// Hex dump utility for debugging
void print_hex_dump(const void *data, size_t size) {
    const unsigned char *bytes = (const unsigned char *)data;

    for (size_t i = 0; i < size; i++) {
        if (i % 16 == 0) {
            if (i > 0) {
                write_char('\n');
            }
            write_hex(i, 1);
            write_str(": ");
        }

        unsigned char byte = bytes[i];
        int high = (byte >> 4) & 0xF;
        int low = byte & 0xF;

        write_char(high < 10 ? ('0' + high) : ('a' + high - 10));
        write_char(low < 10 ? ('0' + low) : ('a' + low - 10));
        write_char(' ');

        if ((i + 1) % 8 == 0) {
            write_char(' ');
        }
    }

    write_char('\n');
}
