# Ch0nky  HTTP Client Implementation 

In this assignment, you will work on implementing a simple HTTP client in C that uses POSIX sockets to make an HTTP client. The project is split into several files 
## File Structure

Create the following files in your project directory:

- **debug.h**: provides simple `DEBUG_LOG` implementation
- **http_client.h** :  This header declares the structures and functions needed for making HTTP requests and handling responses.
- **http_client.c**: Stub code that you must fill in!
- **main.c**: code to test out your program


### 2. `http_client.h`
This header declares the structures and functions needed for making HTTP requests and handling responses.

**Starter Code:**
```c
// http_client.h
#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Default timeout in seconds for HTTP operations.
 */
#define DEFAULT_TIMEOUT_SECONDS 5

/**
 * Default maximum response size (5 MB)
 */
#define DEFAULT_MAX_RESPONSE_SIZE (5 * 1024 * 1024)

/**
 * HTTP request structure.
 */
typedef struct {
  const char *verb;     /**< HTTP method (GET, POST, etc.) */
  const char *host;     /**< Host to connect to */
  const char *path;     /**< Resource path */
  const char **headers; /**< Null-terminated array of headers */
  const void *body;     /**< Request body */
  size_t body_size;     /**< Size of the request body */
  const char *port;     /**< Port (optional, defaults to 80) */
  int timeout_seconds;  /**< Timeout in seconds (0 for default) */
} HttpRequest;

/**
 * HTTP header key-value pair.
 */
typedef struct {
  char *key;   /**< Header name */
  char *value; /**< Header value */
} HttpHeader;

/**
 * HTTP response structure.
 */
typedef struct {
  int status_code;     /**< HTTP status code */
  HttpHeader *headers; /**< Null-terminated array of headers */
  void *body;          /**< Response body */
  size_t body_size;    /**< Size of the response body */
} HttpResponse;

/**
 * Make an HTTP request.
 *
 * @param request The HTTP request configuration.
 * @param max_response_size Maximum size of response to accept (0 for default).
 * @return HttpResponse* Response object (must be freed with free_http_response) or NULL on error.
 */
HttpResponse *make_http_request(const HttpRequest *request,
                                size_t max_response_size);

/**
 * Free an HTTP response and all associated memory.
 *
 * @param response The response to free.
 */
void free_http_response(HttpResponse *response);

#ifdef __cplusplus
}
#endif

#endif // HTTP_CLIENT_H
```


###  `http_client.c`
This file contains the complete implementation of the HTTP client functions:
- Establishing a connection (with timeouts).
- Building the HTTP request string.
- Sending the request and reading the response.
- Parsing the HTTP response (status line, headers, and body).
- Cleaning up allocated resources.

## **Starter Code:**
To help break down this task, we provide several helper functions for you to implement:

- ., `open_connection`, `set_socket_timeout`, `build_request_string`, `parse_response`, `trim_string`, and `strdup_safe`.
- Debug logging is used to output errors or status messages.
- Memory allocated for the response and its components is correctly freed by `free_http_response`.

---

### 4. `main.c`
This file demonstrates example usage of the HTTP client. It constructs an HTTP GET request to a server (e.g., `example.com`), sends the request, and prints the received response.

**Starter Code:**
```c
#include "debug.h"
#include "http_client.h"
#include <unistd.h>

int example_usage() {
  // Define request headers.
  const char *headers[] = {"Accept: */*", "User-Agent: SimpleHttpClient/1.0",
                           "Connection: close", NULL};

  // Create an HTTP request.
  HttpRequest request = {
      .verb = "GET",
      .host = "example.com",
      .path = "/",
      .headers = headers,
      .body = NULL,
      .body_size = 0,
      .port = "80",
      .timeout_seconds = 5
  };

  // Make the HTTP request.
  HttpResponse *response =
      make_http_request(&request, DEFAULT_MAX_RESPONSE_SIZE);
  if (!response) {
    DEBUG_LOG("Failed to get response");
    return 1;
  }

  // Print the response details.
  DEBUG_LOG("Status code: %d", response->status_code);
  DEBUG_LOG("Headers:");
  for (int i = 0; response->headers[i].key != NULL; i++) {
    DEBUG_LOG("%s: %s", response->headers[i].key, response->headers[i].value);
  }
  DEBUG_LOG("Body size: %zu bytes", response->body_size);
  write(1, response->body, response->body_size);

  // Clean up.
  free_http_response(response);
  return 0;
}

int main() {
  return example_usage();
}
```

---

## BUILD 

use the Makefile
```bash
gcc -DDEBUG -o http_client main.c http_client.c -Wall -Wextra
```

**Explanation of Flags:**
- `-DDEBUG`: Enables the debug logging by defining the `DEBUG` macro.
- `-Wall -Wextra`: Enable extra compiler warnings to help catch potential issues.
- `-o http_client`: Specifies the output executable name.

---

## Running the Application

After compiling successfully, run the executable:

```bash
./http_client
```

The program will send an HTTP GET request to `example.com` and display the status code, headers, and body of the response via the debug log.

---

## Additional Implementation Directions

- **Error Handling:**  
  The code uses `DEBUG_LOG` to report errors such as memory allocation failures, socket connection issues, or data transmission errors. Study these log messages to understand how errors are detected and handled.

- **Memory Management:**  
  Pay attention to dynamic memory allocation. The function `free_http_response` is critical to avoid memory leaks by freeing all allocated memory for headers and the response body.

- **Socket Programming:**  
  The HTTP client uses non-blocking sockets and timeouts. The functions `open_connection` and `set_socket_timeout` manage connection timeouts, which is essential for robust network programming.

- **Extensibility:**  
  The code is designed as a starting point. Consider extending it to:
  - Support additional HTTP methods (e.g., POST, PUT, DELETE).
  - Handle chunked transfer encoding.
  - Provide more robust parsing of HTTP headers and bodies.
  - Improve error handling and response parsing.

