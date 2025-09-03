# Advanced Network Programming Assignment

## Overview

This assignment is designed to develop practical expertise in network programming through a series of progressive implementation tasks. You will work with both Python and C to gain hands-on experience with fundamental networking concepts and security protocols.

### Learning Objectives

By completing this assignment, you will:
- Implement TCP socket programming in both Python and C
- Create HTTP server and client applications using various technologies
- Configure TLS/HTTPS with certificate validation
- Apply cryptographic principles for secure communication

### Prerequisites

Ensure you have the following development environment prepared:
- Python 3.8+ with pip
- GCC 9.0+ or equivalent C compiler
- Required libraries:
  - Python: Flask, PyNaCl
  - C: OpenSSL development libraries, libcurl, libsodium

## Assignment Structure

Each part builds upon concepts introduced in previous sections, progressively increasing in complexity:

1. **TCP Socket Programming** - Fundamentals of connection-oriented communication
2. **HTTP Implementation** - Application layer protocol development
3. **TLS Integration** - Secure transport layer implementation
4. **HTTPS Applications** - Combining HTTP with TLS security
5. **Advanced Cryptography** - Key exchange and encrypted session management

---

## Part 1: TCP Socket Programming

### Section 1.1: TCP Echo Server (Python)

#### Objective
Implement a TCP server that accepts connections on a specified port, receives data from clients, and echoes the received data back to the client.

#### Requirements
- Create a TCP socket that binds to localhost (127.0.0.1) on port 65432
- Listen for and accept incoming connections
- Receive data in chunks of appropriate size
- Echo received data back to the client
- Implement proper connection handling and cleanup
- Include error handling for common failure cases

#### Implementation
```python
#!/usr/bin/env python3
"""
TCP Echo Server Implementation
This program demonstrates basic socket programming by creating a server that
echoes back any data it receives from clients.
"""
import socket
import logging
from typing import Tuple

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s'
)

# Server configuration
HOST = '127.0.0.1'
PORT = 65432
BUFFER_SIZE = 1024

def handle_client(conn: socket.socket, addr: Tuple[str, int]) -> None:
    """
    Handle an individual client connection by echoing received data.
    
    Args:
        conn: The client connection socket
        addr: The client address information
    """
    try:
        logging.info(f"Connected to client at {addr}")
        
        while True:
            # Receive data from the client
            data = conn.recv(BUFFER_SIZE)
            if not data:
                break
                
            # Log received data (truncated if too long for display)
            data_preview = data[:50].decode('utf-8', errors='replace')
            if len(data) > 50:
                data_preview += "..."
            logging.info(f"Received: {data_preview}")
            
            # Echo data back to client
            conn.sendall(data)
            
        logging.info(f"Client {addr} disconnected")
    except Exception as e:
        logging.error(f"Error handling client {addr}: {e}")
    finally:
        conn.close()

def main() -> None:
    """
    Main server function that initializes the socket and handles connections.
    """
    # Create a TCP/IP socket
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    
    # Set socket option to allow port reuse (helps with socket in TIME_WAIT state)
    server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    
    try:
        # Bind the socket to the address and port
        server_socket.bind((HOST, PORT))
        
        # Enable the server to accept connections
        server_socket.listen(5)
        logging.info(f"Echo server listening on {HOST}:{PORT}")
        
        while True:
            # Wait for a connection
            client_conn, client_addr = server_socket.accept()
            handle_client(client_conn, client_addr)
            
    except KeyboardInterrupt:
        logging.info("Server shutting down gracefully")
    except Exception as e:
        logging.error(f"Server error: {e}")
    finally:
        server_socket.close()
        logging.info("Server socket closed")

if __name__ == '__main__':
    main()
```

#### Testing Instructions
1. Save the code as `echo_server.py`
2. Make the file executable: `chmod +x echo_server.py`
3. Run the server: `./echo_server.py`
4. In another terminal, connect to the server using netcat:
   ```
   nc 127.0.0.1 65432
   ```
5. Type messages and observe them being echoed back

---

### Section 1.2: TCP Client (C)

#### Objective
Develop a C-based TCP client program that connects to a server, sends user input, and displays server responses.

#### Requirements
- Create a TCP socket using POSIX socket API
- Connect to a server based on command-line parameters (IP address and port)
- Send user input from stdin to the server
- Receive and display server responses
- Implement clean shutdown procedures
- Include comprehensive error handling

#### Implementation
```c
/**
 * Main function
 */
int main(int argc, char *argv[]) {
    // Initialize libcurl
    curl_global_init(CURL_GLOBAL_DEFAULT);
    
    int result = EXIT_FAILURE;
    
    if (argc < 3) {
        usage(argv[0]);
        goto cleanup;
    }
    
    if (strcmp(argv[1], "get") == 0) {
        result = perform_http_get(argv[2]) == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
    } else if (strcmp(argv[1], "post") == 0) {
        if (argc < 4) {
            printf("Error: POST requires a JSON data parameter\n");
            usage(argv[0]);
            goto cleanup;
        }
        result = perform_http_post_json(argv[2], argv[3]) == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
    } else {
        printf("Unknown command: %s\n", argv[1]);
        usage(argv[0]);
    }
    
cleanup:
    curl_global_cleanup();
    return result;
}
```

#### Compilation and Testing
1. Save the code as `https_client.c`
2. Compile:
   ```
   gcc -Wall -Wextra -o https_client https_client.c -lcurl
   ```
3. Test with various options:
   ```
   # Simple GET request
   ./https_client https://example.com
   
   # Verbose output
   ./https_client -v https://example.com
   
   # POST request with JSON data
   ./https_client -X POST -d '{"message":"Hello"}' https://httpbin.org/post
   ```

---

### Section 4.2: HTTPS Server with Python Flask

#### Objective
Implement an HTTPS server using Flask and self-signed certificates.

#### Requirements
- Create a Flask application that serves content over HTTPS
- Generate and configure a self-signed certificate
- Implement proper TLS configuration
- Support both GET and POST endpoints
- Implement appropriate error handling

#### Certificate Generation
Generate a self-signed certificate for the HTTPS server:

```bash
# Create a directory for certificates
mkdir -p https-certs

# Generate a 2048-bit RSA private key and self-signed certificate
openssl req -x509 -newkey rsa:2048 -nodes \
  -keyout https-certs/key.pem -out https-certs/cert.pem \
  -subj "/C=US/ST=State/L=City/O=Organization/CN=localhost" \
  -days 365
```

#### Implementation
```python
#!/usr/bin/env python3
"""
HTTPS Server Implementation with Flask

This program demonstrates how to create an HTTPS server using Flask with
a self-signed certificate for secure communication.
"""
import os
import json
import logging
from datetime import datetime
from flask import Flask, request, jsonify, render_template_string, abort

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s'
)

# Create Flask application
app = Flask(__name__)

# Simple HTML template
HTML_TEMPLATE = """
<!DOCTYPE html>
<html>
<head>
    <title>HTTPS Server Demo</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            max-width: 800px;
            margin: 0 auto;
            padding: 20px;
            line-height: 1.6;
        }
        h1 {
            color: #333;
        }
        .endpoint {
            background-color: #f5f5f5;
            border-left: 4px solid #28a745;
            padding: 10px;
            margin-bottom: 10px;
        }
        code {
            background-color: #eee;
            padding: 2px 4px;
            border-radius: 4px;
        }
        .secure {
            color: #28a745;
            font-weight: bold;
        }
    </style>
</head>
<body>
    <h1>HTTPS Server Demo</h1>
    <p>This server demonstrates a secure implementation using <span class="secure">HTTPS</span>.</p>
    
    <div class="endpoint">
        <h3>GET /</h3>
        <p>The page you're currently viewing.</p>
    </div>
    
    <div class="endpoint">
        <h3>GET /api/info</h3>
        <p>Returns server information as JSON.</p>
        <p>Try it: <a href="/api/info">/api/info</a></p>
    </div>
    
    <div class="endpoint">
        <h3>POST /api/data</h3>
        <p>Accepts and returns JSON data.</p>
        <p>Example using curl:</p>
        <code>curl -k -X POST -H "Content-Type: application/json" -d '{"message":"Hello"}' https://localhost:4443/api/data</code>
    </div>
</body>
</html>
"""

@app.route('/')
def index():
    """Serve the index page with API documentation."""
    return render_template_string(HTML_TEMPLATE)

@app.route('/api/info')
def info():
    """Return server information as JSON."""
    info_data = {
        "server": "Flask HTTPS Demo Server",
        "timestamp": datetime.now().isoformat(),
        "secure": True,
        "client_info": {
            "ip": request.remote_addr,
            "user_agent": request.headers.get("User-Agent", "Unknown"),
        },
        "endpoints": ["/", "/api/info", "/api/data"]
    }
    return jsonify(info_data)

@app.route('/api/data', methods=['POST'])
def data():
    """Handle POST requests with JSON data."""
    if not request.is_json:
        return jsonify({"error": "Request must be JSON"}), 400
    
    # Log request data
    logging.info(f"Received data: {request.json}")
    
    # Process the data (in this case, just echo it back with a timestamp)
    response_data = {
        "received_data": request.json,
        "timestamp": datetime.now().isoformat(),
        "status": "success"
    }
    
    return jsonify(response_data)

@app.errorhandler(404)
def not_found(e):
    """Handle 404 errors."""
    return jsonify({"error": "Resource not found"}), 404

@app.errorhandler(500)
def server_error(e):
    """Handle 500 errors."""
    return jsonify({"error": "Internal server error"}), 500

def main():
    """Run the HTTPS server."""
    cert_path = os.path.join('https-certs', 'cert.pem')
    key_path = os.path.join('https-certs', 'key.pem')
    
    # Check if certificate and key files exist
    if not os.path.exists(cert_path) or not os.path.exists(key_path):
        logging.error("Certificate or key file not found. Please generate them first.")
        logging.info("Generate using: openssl req -x509 -newkey rsa:2048 -nodes "
                    f"-keyout {key_path} -out {cert_path} "
                    "-subj '/C=US/ST=State/L=City/O=Organization/CN=localhost' "
                    "-days 365")
        return
    
    # Run the Flask app with HTTPS
    logging.info(f"Starting HTTPS server on port 4443")
    logging.info(f"Using certificate: {cert_path}")
    logging.info(f"Using key: {key_path}")
    
    app.run(
        host='0.0.0.0',
        port=4443,
        ssl_context=(cert_path, key_path),
        debug=False
    )

if __name__ == '__main__':
    main()
```

#### Testing Instructions
1. Save the code as `https_server.py`
2. Ensure you have generated the certificate and key files
3. Install Flask if needed: `pip install flask`
4. Run the server: `python https_server.py`
5. Test with a browser by navigating to https://localhost:4443/
6. Test with curl (note the `-k` flag to ignore certificate validation):
   ```
   curl -k https://localhost:4443/api/info
   curl -k -X POST -H "Content-Type: application/json" -d '{"message":"Hello"}' https://localhost:4443/api/data
   ```
7. Test with your C HTTPS client:
   ```
   ./https_client -k https://localhost:4443/api/info
   ./https_client -k -X POST -d '{"message":"Hello"}' https://localhost:4443/api/data
   ```

---

## Part 5: Advanced Cryptography with Libsodium

### Section 5.1: Key Exchange and Encrypted Communication

#### Objective
Implement secure communication between a Python server and a C client using modern cryptographic techniques.

#### Requirements
- Implement a Python server that uses PyNaCl for cryptography
- Create a C client that uses libsodium for cryptography
- Perform key exchange using X25519
- Establish a shared secret
- Encrypt and decrypt messages with authenticated encryption
- Implement proper key management and nonce handling

#### PyNaCl Server Implementation
```python
#!/usr/bin/env python3
"""
Secure Communication Server using PyNaCl

This program demonstrates secure communication using modern cryptography,
specifically X25519 key exchange and XSalsa20-Poly1305 authenticated encryption.
"""
import socket
import logging
import binascii
from nacl.public import PrivateKey, Box
from nacl.utils import random

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s'
)

# Server configuration
HOST = '127.0.0.1'
PORT = 6000
BUFFER_SIZE = 1024

def generate_keypair():
    """Generate X25519 keypair for key exchange."""
    # Generate a new random private key
    private_key = PrivateKey.generate()
    # Get the public key
    public_key = private_key.public_key
    
    logging.info(f"Generated new keypair")
    logging.info(f"Public key: {binascii.hexlify(public_key.encode()).decode()}")
    
    return private_key, public_key

def process_client(client_socket, server_private_key):
    """Process client connection with secure communication."""
    try:
        # Send server's public key
        server_public_key_bytes = server_private_key.public_key.encode()
        client_socket.sendall(server_public_key_bytes)
        logging.info(f"Sent server public key: {binascii.hexlify(server_public_key_bytes).decode()}")
        
        # Receive client's public key
        client_public_key_bytes = client_socket.recv(BUFFER_SIZE)
        if len(client_public_key_bytes) != 32:  # X25519 public key is 32 bytes
            logging.error(f"Received invalid public key length: {len(client_public_key_bytes)}")
            return
        
        logging.info(f"Received client public key: {binascii.hexlify(client_public_key_bytes).decode()}")
        
        # Create Box for encryption/decryption using the shared secret
        box = Box(server_private_key, client_public_key_bytes)
        
        # Receive encrypted message from client
        encrypted_message = client_socket.recv(BUFFER_SIZE)
        logging.info(f"Received encrypted message ({len(encrypted_message)} bytes)")
        
        # Decrypt the message
        decrypted_message = box.decrypt(encrypted_message)
        logging.info(f"Decrypted message: {decrypted_message.decode('utf-8')}")
        
        # Encrypt and send a response
        response = f"I received your message: {decrypted_message.decode('utf-8')}"
        encrypted_response = box.encrypt(response.encode('utf-8'))
        client_socket.sendall(encrypted_response)
        logging.info(f"Sent encrypted response ({len(encrypted_response)} bytes)")
        
    except Exception as e:
        logging.error(f"Error processing client: {e}")

def main():
    """Run the secure communication server."""
    # Generate server's keypair
    server_private_key, server_public_key = generate_keypair()
    
    # Create TCP socket
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    
    try:
        # Bind and listen
        server_socket.bind((HOST, PORT))
        server_socket.listen(5)
        logging.info(f"Server listening on {HOST}:{PORT}")
        
        while True:
            # Accept client connection
            client_socket, addr = server_socket.accept()
            logging.info(f"Client connected: {addr[0]}:{addr[1]}")
            
            # Process client in the same thread (for simplicity)
            process_client(client_socket, server_private_key)
            
            # Close client connection
            client_socket.close()
            logging.info(f"Client connection closed")
            
    except KeyboardInterrupt:
        logging.info("Server shutting down")
    except Exception as e:
        logging.error(f"Server error: {e}")
    finally:
        server_socket.close()

if __name__ == '__main__':
    main()
```

#### Libsodium Client Implementation
```c
/**
 * secure_client.c - Secure communication client using libsodium
 * 
 * This program demonstrates secure communication using modern cryptography,
 * specifically X25519 key exchange and XSalsa20-Poly1305 authenticated encryption.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sodium.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 6000
#define BUFFER_SIZE 1024

/**
 * Print binary data in hexadecimal format
 */
void print_hex(const char *label, const unsigned char *data, size_t len) {
    printf("%s: ", label);
    for (size_t i = 0; i < len; i++) {
        printf("%02x", data[i]);
    }
    printf("\n");
}

/**
 * Create and connect a socket to the server
 */
int connect_to_server() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Error creating socket");
        return -1;
    }
    
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("Invalid address or address not supported");
        close(sock);
        return -1;
    }
    
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(sock);
        return -1;
    }
    
    printf("Connected to %s:%d\n", SERVER_IP, SERVER_PORT);
    return sock;
}

/**
 * Main function
 */
int main(int argc, char *argv[]) {
    // Message to encrypt and send
    const char *message = "Hello, secure world!";
    if (argc > 1) {
        message = argv[1];
    }
    
    // Initialize libsodium
    if (sodium_init() < 0) {
        fprintf(stderr, "Error initializing libsodium\n");
        return EXIT_FAILURE;
    }
    
    // Connect to server
    int sock = connect_to_server();
    if (sock < 0) {
        return EXIT_FAILURE;
    }
    
    // Generate client's keypair for X25519
    unsigned char client_pk[crypto_box_PUBLICKEYBYTES];
    unsigned char client_sk[crypto_box_SECRETKEYBYTES];
    crypto_box_keypair(client_pk, client_sk);
    
    print_hex("Client public key", client_pk, crypto_box_PUBLICKEYBYTES);
    
    // Receive server's public key
    unsigned char server_pk[crypto_box_PUBLICKEYBYTES];
    ssize_t bytes_received = recv(sock, server_pk, sizeof(server_pk), 0);
    if (bytes_received != crypto_box_PUBLICKEYBYTES) {
        fprintf(stderr, "Error receiving server public key: received %zd bytes\n", bytes_received);
        close(sock);
        return EXIT_FAILURE;
    }
    
    print_hex("Server public key", server_pk, crypto_box_PUBLICKEYBYTES);
    
    // Send client's public key
    if (send(sock, client_pk, crypto_box_PUBLICKEYBYTES, 0) != crypto_box_PUBLICKEYBYTES) {
        perror("Error sending client public key");
        close(sock);
        return EXIT_FAILURE;
    }
    
    // Compute shared secret using crypto_box_beforenm
    unsigned char shared_key[crypto_box_BEFORENMBYTES];
    if (crypto_box_beforenm(shared_key, server_pk, client_sk) != 0) {
        fprintf(stderr, "Error computing shared key\n");
        close(sock);
        return EXIT_FAILURE;
    }
    
    // Generate random nonce
    unsigned char nonce[crypto_box_NONCEBYTES];
    randombytes_buf(nonce, sizeof(nonce));
    
    // Encrypt message
    size_t message_len = strlen(message);
    unsigned char ciphertext[crypto_box_MACBYTES + message_len];
    if (crypto_box_easy_afternm(ciphertext, (const unsigned char *)message, 
                              message_len, nonce, shared_key) != 0) {
        fprintf(stderr, "Encryption failed\n");
        close(sock);
        return EXIT_FAILURE;
    }
    
    // Prepare packet: nonce + ciphertext
    unsigned char packet[crypto_box_NONCEBYTES + sizeof(ciphertext)];
    memcpy(packet, nonce, crypto_box_NONCEBYTES);
    memcpy(packet + crypto_box_NONCEBYTES, ciphertext, sizeof(ciphertext));
    
    // Send encrypted message
    size_t packet_len = crypto_box_NONCEBYTES + crypto_box_MACBYTES + message_len;
    if (send(sock, packet, packet_len, 0) != packet_len) {
        perror("Error sending encrypted message");
        close(sock);
        return EXIT_FAILURE;
    }
    
    printf("Sent encrypted message (%zu bytes): \"%s\"\n", packet_len, message);
    
    // Receive encrypted response
    unsigned char response_packet[BUFFER_SIZE];
    bytes_received = recv(sock, response_packet, sizeof(response_packet), 0);
    if (bytes_received <= (crypto_box_NONCEBYTES + crypto_box_MACBYTES)) {
        fprintf(stderr, "Error receiving server response or response too short\n");
        close(sock);
        return EXIT_FAILURE;
    }
    
    // Split packet into nonce and ciphertext
    unsigned char response_nonce[crypto_box_NONCEBYTES];
    memcpy(response_nonce, response_packet, crypto_box_NONCEBYTES);
    
    size_t response_ciphertext_len = bytes_received - crypto_box_NONCEBYTES;
    unsigned char response_ciphertext[response_ciphertext_len];
    memcpy(response_ciphertext, response_packet + crypto_box_NONCEBYTES, response_ciphertext_len);
    
    // Decrypt response
    unsigned char decrypted[BUFFER_SIZE];
    if (crypto_box_open_easy_afternm(decrypted, response_ciphertext, response_ciphertext_len,
                                    response_nonce, shared_key) != 0) {
        fprintf(stderr, "Decryption failed\n");
        close(sock);
        return EXIT_FAILURE;
    }
    
    // Null-terminate decrypted message
    decrypted[response_ciphertext_len - crypto_box_MACBYTES] = '\0';
    
    printf("Received decrypted response: \"%s\"\n", decrypted);
    
    // Clean up
    close(sock);
    sodium_memzero(client_sk, sizeof(client_sk));  // Securely erase secret key
    sodium_memzero(shared_key, sizeof(shared_key));  // Securely erase shared key
    
    return EXIT_SUCCESS;
}
```

#### Compilation and Testing Instructions

##### For the Python Server
1. Save the code as `secure_server.py`
2. Install PyNaCl if needed: `pip install pynacl`
3. Run the server: `python secure_server.py`

##### For the C Client
1. Save the code as `secure_client.c`
2. Install libsodium development package if needed:
   ```
   sudo apt-get install libsodium-dev  # Debian/Ubuntu
   sudo yum install libsodium-devel    # CentOS/RHEL
   ```
3. Compile:
   ```
   gcc -Wall -Wextra -o secure_client secure_client.c -lsodium
   ```
4. Run the client:
   ```
   ./secure_client "This is a secret message"
   ```

---

## Comprehensive Testing Guide

### Setting Up the Test Environment

1. **Create a dedicated directory for the assignment:**
   ```bash
   mkdir network_programming_assignment
   cd network_programming_assignment
   ```

2. **Create subdirectories for each part:**
   ```bash
   mkdir part1 part2 part3 part4 part5
   ```

3. **Copy the relevant files to each subdirectory**

4. **Verify dependencies are installed:**
   ```bash
   # Python dependencies
   pip install flask pynacl

   # C dependencies (Debian/Ubuntu)
   sudo apt-get install build-essential libssl-dev libcurl4-openssl-dev libsodium-dev
   
   # C dependencies (CentOS/RHEL)
   sudo yum groupinstall "Development Tools"
   sudo yum install openssl-devel libcurl-devel libsodium-devel
   ```

### Testing Matrix

#### Part 1: TCP Socket Programming
| Test Case                | Command                              | Expected Output                       |
| ------------------------ | ------------------------------------ | ------------------------------------- |
| Start Echo Server        | `python part1/echo_server.py`        | Server listening message              |
| Connect with Client      | `./part1/tcp_client 127.0.0.1 65432` | Connected message, echo functionality |
| Test with Large Data     | Send 10KB of text                    | Complete echo of data                 |
| Test Connection Handling | Connect multiple clients             | Server handles all connections        |

#### Part 2: HTTP Implementation
| Test Case                 | Command                                                   | Expected Output                |
| ------------------------- | --------------------------------------------------------- | ------------------------------ |
| Start Flask Server        | `python part2/flask_server.py`                            | Server running message         |
| Test GET with Raw Client  | `./part2/http_client get /api/status`                     | Status code 200, JSON response |
| Test POST with Raw Client | `./part2/http_client post /api/echo '{"message":"test"}'` | Echo of JSON data              |
| Test with LibCurl Client  | `./part2/libcurl_client get /api/status`                  | Status code 200, JSON response |

#### Part 3: TLS Implementation
| Test Case                     | Command                                    | Expected Output                  |
| ----------------------------- | ------------------------------------------ | -------------------------------- |
| Generate Certificates         | `openssl` commands                         | Certificate files created        |
| Start TLS Server              | `./part3/tls_server`                       | Server listening message         |
| Test with TLS Client          | `./part3/tls_client`                       | Secure connection, data exchange |
| Test Certificate Verification | `./part3/tls_client -h example.com -p 443` | Verification success or failure  |

#### Part 4: HTTPS Implementation
| Test Case                   | Command                                                                             | Expected Output        |
| --------------------------- | ----------------------------------------------------------------------------------- | ---------------------- |
| Start HTTPS Server          | `python part4/https_server.py`                                                      | Server running message |
| Test GET with HTTPS Client  | `./part4/https_client https://localhost:4443/api/info`                              | JSON response          |
| Test POST with HTTPS Client | `./part4/https_client -X POST -d '{"key":"value"}' https://localhost:4443/api/data` | Echo of JSON data      |
| Test with curl              | `curl -k https://localhost:4443/api/info`                                           | JSON response          |

#### Part 5: Advanced Cryptography
| Test Case                    | Command                                  | Expected Output                        |
| ---------------------------- | ---------------------------------------- | -------------------------------------- |
| Start Secure Server          | `python part5/secure_server.py`          | Server listening message               |
| Test with Secure Client      | `./part5/secure_client "Secret message"` | Encrypted exchange, decrypted response |
| Test Message Integrity       | Modify transmitted data                  | Decryption failure                     |
| Test with Different Messages | Various message lengths                  | Successful encryption/decryption       |

### Troubleshooting Common Issues

#### Connection Problems
- Verify that servers are running and listening on the correct port
- Check for firewall rules that might block connections
- Ensure the correct IP address is being used (especially important for TLS hostname verification)

#### TLS/SSL Errors
- Verify certificate/key paths are correct
- Check certificate validity and permissions
- For hostname verification issues, ensure the certificate's CN matches the hostname

#### Encryption/Decryption Failures
- Verify correct key exchange process
- Check for proper nonce handling
- Ensure message padding and formatting is correct

#### Library Dependencies
- Verify all libraries are installed and have compatible versions
- Check for correct linking during compilation
- For Python, ensure virtual environments have the necessary packages

---

## Submission Guidelines

### Required Files

1. **Source Code Files:**
   - All implementation files as specified in each section
   - Makefiles or build scripts for C programs
   - Python scripts for server implementations

2. **Documentation:**
   - A detailed README.md file explaining the project structure and how to test each component
   - Comments within the code explaining key concepts and implementation choices
   - A testing report documenting the results of your tests

3. **Certificates:**
   - Include the commands to generate certificates, but NOT the actual certificate files

### Code Organization

Organize your submission in a logical directory structure:

```
submission/
├── README.md
├── part1/
│   ├── echo_server.py
│   ├── tcp_client.c
│   └── Makefile
├── part2/
│   ├── flask_server.py
│   ├── http_client.c
│   ├── libcurl_client.c
│   └── Makefile
├── part3/
│   ├── tls_server.c
│   ├── tls_client.c
│   └── Makefile
├── part4/
│   ├── https_server.py
│   ├── https_client.c
│   └── Makefile
└── part5/
    ├── secure_server.py
    ├── secure_client.c
    └── Makefile
```

### Evaluation Criteria

Your submission will be evaluated based on:

1. **Correctness:** Do all components work as specified?
2. **Security:** Are proper security practices followed for TLS and encryption?
3. **Error Handling:** Does the code handle errors gracefully?
4. **Documentation:** Is the code well-documented and easy to understand?
5. **Testing:** Have you thoroughly tested each component?

For questions or clarification, please reach out to the course instructor or teaching assistants.


#### Compilation and Testing
1. Save the code as `libcurl_client.c`
2. Install libcurl development package if needed:
   ```
   sudo apt-get install libcurl4-openssl-dev  # Debian/Ubuntu
   sudo yum install libcurl-devel             # CentOS/RHEL
   ```
3. Compile:
   ```
   gcc -Wall -Wextra -o libcurl_client libcurl_client.c -lcurl
   ```
4. Test with:
   ```
   ./libcurl_client get /api/status
   ./libcurl_client post /api/echo '{"message":"Hello from libcurl"}'
   ```

---

## Part 3: TLS Implementation

### Section 3.1: TLS Server with Self-Signed Certificate

#### Objective
Create a TLS server that uses a self-signed certificate to secure communication.

#### Requirements
- Generate a self-signed certificate and private key
- Set up a secure TCP server using OpenSSL
- Implement proper TLS handshaking
- Handle encrypted data transmission
- Configure cipher suites appropriately

#### Certificate Generation

Before implementing the TLS server, generate a self-signed certificate:

```bash
# Create a directory for certificates
mkdir -p certs

# Generate a 2048-bit RSA private key and self-signed certificate
openssl req -x509 -newkey rsa:2048 -nodes \
  -keyout certs/key.pem -out certs/cert.pem \
  -subj "/C=US/ST=State/L=City/O=Organization/CN=localhost" \
  -days 365
```

#### Implementation
```c
/**
 * tls_server.c - TLS Server implementation using OpenSSL
 * 
 * This program demonstrates how to create a secure server using TLS/SSL
 * with a self-signed certificate.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <signal.h>

#define PORT 4433
#define BUFFER_SIZE 1024
#define CERT_FILE "certs/cert.pem"
#define KEY_FILE "certs/key.pem"

// Global variables for cleanup
SSL_CTX *g_ctx = NULL;
int g_server_fd = -1;

/**
 * Initialize OpenSSL library
 */
void init_openssl() {
    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();
}

/**
 * Clean up OpenSSL
 */
void cleanup_openssl() {
    EVP_cleanup();
}

/**
 * Create SSL context for server
 */
SSL_CTX *create_ssl_context() {
    const SSL_METHOD *method;
    SSL_CTX *ctx;
    
    // Use TLS method for server
    method = TLS_server_method();
    
    // Create new SSL context
    ctx = SSL_CTX_new(method);
    if (!ctx) {
        perror("Unable to create SSL context");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
    
    return ctx;
}

/**
 * Configure the SSL context with certificate and private key
 */
void configure_context(SSL_CTX *ctx) {
    // Set the certificate
    if (SSL_CTX_use_certificate_file(ctx, CERT_FILE, SSL_FILETYPE_PEM) <= 0) {
        fprintf(stderr, "Error loading certificate from %s\n", CERT_FILE);
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
    
    // Set the private key
    if (SSL_CTX_use_PrivateKey_file(ctx, KEY_FILE, SSL_FILETYPE_PEM) <= 0) {
        fprintf(stderr, "Error loading private key from %s\n", KEY_FILE);
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
    
    // Verify private key
    if (!SSL_CTX_check_private_key(ctx)) {
        fprintf(stderr, "Private key does not match the certificate\n");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
    
    // Set recommended cipher list (TLSv1.2 and above)
    if (!SSL_CTX_set_cipher_list(ctx, "HIGH:!aNULL:!MD5:!RC4")) {
        fprintf(stderr, "Error setting cipher list\n");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
}

/**
 * Handle client connection
 */
void handle_client(SSL *ssl) {
    char buffer[BUFFER_SIZE];
    int bytes;
    
    // Read client data
    bytes = SSL_read(ssl, buffer, sizeof(buffer) - 1);
    if (bytes > 0) {
        buffer[bytes] = '\0';
        printf("Received: %s\n", buffer);
        
        // Echo back
        SSL_write(ssl, buffer, bytes);
    } else {
        // Check SSL errors
        int err = SSL_get_error(ssl, bytes);
        if (err == SSL_ERROR_ZERO_RETURN) {
            printf("Client closed connection\n");
        } else {
            fprintf(stderr, "SSL read failed: %d\n", err);
            ERR_print_errors_fp(stderr);
        }
    }
}

/**
 * Signal handler for graceful termination
 */
void handle_signal(int sig) {
    printf("\nShutting down server...\n");
    if (g_ctx != NULL) {
        SSL_CTX_free(g_ctx);
        g_ctx = NULL;
    }
    if (g_server_fd != -1) {
        close(g_server_fd);
        g_server_fd = -1;
    }
    cleanup_openssl();
    exit(EXIT_SUCCESS);
}

/**
 * Main server function
 */
int main() {
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    int server_fd;
    SSL *ssl;
    
    // Set up signal handler
    signal(SIGINT, handle_signal);
    
    // Initialize OpenSSL
    init_openssl();
    
    // Create SSL context and configure it
    g_ctx = create_ssl_context();
    configure_context(g_ctx);
    
    // Create server socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Unable to create socket");
        exit(EXIT_FAILURE);
    }
    g_server_fd = server_fd;
    
    // Set socket option for address reuse
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt SO_REUSEADDR failed");
        exit(EXIT_FAILURE);
    }
    
    // Configure server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);
    
    // Bind socket to address and port
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Unable to bind");
        exit(EXIT_FAILURE);
    }
    
    // Listen for incoming connections
    if (listen(server_fd, 5) < 0) {
        perror("Unable to listen");
        exit(EXIT_FAILURE);
    }
    
    printf("TLS server listening on port %d...\n", PORT);
    printf("Press Ctrl+C to stop the server\n");
    
    // Main server loop
    while(1) {
        // Accept incoming connection
        int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd < 0) {
            perror("Unable to accept");
            continue;
        }
        
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
        printf("Client connected: %s:%d\n", client_ip, ntohs(client_addr.sin_port));
        
        // Create new SSL structure for this connection
        ssl = SSL_new(g_ctx);
        SSL_set_fd(ssl, client_fd);
        
        // Perform SSL handshake
        if (SSL_accept(ssl) <= 0) {
            fprintf(stderr, "SSL handshake failed\n");
            ERR_print_errors_fp(stderr);
        } else {
            // Print connection info
            printf("SSL connection established with %s encryption\n", SSL_get_cipher(ssl));
            
            // Handle client
            handle_client(ssl);
        }
        
        // Cleanup this connection
        SSL_shutdown(ssl);
        SSL_free(ssl);
        close(client_fd);
        printf("Connection closed\n");
    }
    
    // Cleanup (in practice, this is handled by signal handler)
    SSL_CTX_free(g_ctx);
    close(server_fd);
    cleanup_openssl();
    
    return 0;
}
```

#### Compilation and Testing
1. Save the code as `tls_server.c`
2. Install OpenSSL development package if needed:
   ```
   sudo apt-get install libssl-dev  # Debian/Ubuntu
   sudo yum install openssl-devel   # CentOS/RHEL
   ```
3. Compile:
   ```
   gcc -Wall -Wextra -o tls_server tls_server.c -lssl -lcrypto
   ```
4. Run the server:
   ```
   ./tls_server
   ```
5. Test with OpenSSL client:
   ```
   echo "Hello, secure world!" | openssl s_client -connect localhost:4433 -ign_eof
   ```

---

### Section 3.2: TLS Client with Certificate Verification

#### Objective
Implement a TLS client that connects to secure servers with proper certificate verification.

#### Requirements
- Create a client that can establish TLS connections
- Implement certificate verification against a trust store
- Handle TLS handshake and secure data exchange
- Allow for customization of security parameters
- Provide informative error messages for certificate issues

#### Implementation
```c
/**
 * tls_client.c - TLS Client implementation with certificate verification
 * 
 * This program demonstrates how to create a secure client using TLS/SSL
 * that verifies server certificates.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/x509v3.h>

#define BUFFER_SIZE 1024
#define DEFAULT_PORT 4433
#define DEFAULT_HOST "localhost"

// Structure to hold client configuration
typedef struct {
    const char *host;
    int port;
    const char *ca_file;
    const char *ca_path;
    int verify_peer;
    int verify_hostname;
} tls_client_config;

/**
 * Initialize OpenSSL library
 */
void init_openssl() {
    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();
}

/**
 * Clean up OpenSSL
 */
void cleanup_openssl() {
    EVP_cleanup();
}

/**
 * Print SSL error details
 */
void print_ssl_error(const char *message) {
    fprintf(stderr, "%s\n", message);
    ERR_print_errors_fp(stderr);
}

/**
 * Verify certificate callback
 */
int verify_callback(int preverify_ok, X509_STORE_CTX *ctx) {
    char buf[256];
    X509 *cert;
    int err, depth;
    
    // Get the certificate and verification error
    cert = X509_STORE_CTX_get_current_cert(ctx);
    err = X509_STORE_CTX_get_error(ctx);
    depth = X509_STORE_CTX_get_error_depth(ctx);
    
    // Get subject name
    X509_NAME_oneline(X509_get_subject_name(cert), buf, sizeof(buf));
    
    if (!preverify_ok) {
        printf("Certificate verification error at depth %d: %s\n", 
               depth, X509_verify_cert_error_string(err));
        printf("Subject: %s\n", buf);
    } else {
        printf("Certificate verified at depth %d: %s\n", depth, buf);
    }
    
    // We could override verification result here, but for now, return original result
    return preverify_ok;
}

/**
 * Create SSL context for client
 */
SSL_CTX *create_ssl_context(tls_client_config *config) {
    const SSL_METHOD *method;
    SSL_CTX *ctx;
    
    // Use TLS method for client
    method = TLS_client_method();
    
    // Create new SSL context
    ctx = SSL_CTX_new(method);
    if (!ctx) {
        print_ssl_error("Unable to create SSL context");
        return NULL;
    }
    
    // Set verification mode
    if (config->verify_peer) {
        SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, verify_callback);
        SSL_CTX_set_verify_depth(ctx, 4);  // How many intermediate certificates to check
        
        // Load trusted certificates
        if (config->ca_file || config->ca_path) {
            if (!SSL_CTX_load_verify_locations(ctx, config->ca_file, config->ca_path)) {
                print_ssl_error("Unable to load CA file or directory");
                SSL_CTX_free(ctx);
                return NULL;
            }
        } else {
            // Use default system locations for CA certificates
            if (!SSL_CTX_set_default_verify_paths(ctx)) {
                print_ssl_error("Unable to load default CA certificates");
                SSL_CTX_free(ctx);
                return NULL;
            }
        }
    } else {
        SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, NULL);
    }
    
    // Set recommended cipher list (TLSv1.2 and above)
    if (!SSL_CTX_set_cipher_list(ctx, "HIGH:!aNULL:!MD5:!RC4")) {
        print_ssl_error("Error setting cipher list");
        SSL_CTX_free(ctx);
        return NULL;
    }
    
    return ctx;
}

/**
 * Create socket and connect to server
 */
int create_socket(const char *host, int port) {
    int sock;
    struct sockaddr_in addr;
    
    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Unable to create socket");
        return -1;
    }
    
    // Configure server address
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    
    // Convert hostname to IP address
    if (inet_pton(AF_INET, host, &addr.sin_addr) <= 0) {
        // If not an IP address, try to resolve hostname
        struct hostent *he = gethostbyname(host);
        if (he == NULL) {
            fprintf(stderr, "Unable to resolve hostname %s\n", host);
            close(sock);
            return -1;
        }
        memcpy(&addr.sin_addr, he->h_addr_list[0], he->h_length);
    }
    
    // Connect to server
    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("Unable to connect");
        close(sock);
        return -1;
    }
    
    return sock;
}

/**
 * Print SSL certificate information
 */
void print_certificate_info(SSL *ssl) {
    X509 *cert;
    char *line;
    
    cert = SSL_get_peer_certificate(ssl);
    if (cert != NULL) {
        printf("Server certificate:\n");
        
        line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        printf("Subject: %s\n", line);
        free(line);
        
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        printf("Issuer: %s\n", line);
        free(line);
        
        X509_free(cert);
    } else {
        printf("No certificate information available\n");
    }
}

/**
 * Main TLS client function
 */
int run_tls_client(tls_client_config *config, const char *message) {
    SSL_CTX *ctx;
    SSL *ssl;
    int sock, result = EXIT_FAILURE;
    char buffer[BUFFER_SIZE];
    
    // Initialize OpenSSL
    init_openssl();
    
    // Create SSL context
    ctx = create_ssl_context(config);
    if (!ctx) {
        goto cleanup_openssl;
    }
    
    // Create socket and connect
    sock = create_socket(config->host, config->port);
    if (sock < 0) {
        goto cleanup_ctx;
    }
    
    // Create new SSL structure
    ssl = SSL_new(ctx);
    if (!ssl) {
        print_ssl_error("Unable to create SSL structure");
        goto cleanup_socket;
    }
    
    // Set hostname verification if requested
    if (config->verify_hostname) {
        SSL_set_hostflags(ssl, X509_CHECK_FLAG_NO_PARTIAL_WILDCARDS);
        if (!SSL_set1_host(ssl, config->host)) {
            print_ssl_error("Unable to set hostname for verification");
            goto cleanup_ssl;
        }
    }
    
    // Attach socket to SSL structure
    SSL_set_fd(ssl, sock);
    
    // Perform TLS handshake
    int ret = SSL_connect(ssl);
    if (ret <= 0) {
        int err = SSL_get_error(ssl, ret);
        fprintf(stderr, "SSL handshake failed (error %d)\n", err);
        ERR_print_errors_fp(stderr);
        goto cleanup_ssl;
    }
    
    // Print connection info
    printf("Connected with %s encryption\n", SSL_get_cipher(ssl));
    print_certificate_info(ssl);
    
    // Verify hostname in certificate
    if (config->verify_hostname) {
        if (SSL_get_verify_result(ssl) != X509_V_OK) {
            fprintf(stderr, "Certificate verification failed\n");
            goto cleanup_ssl;
        }
    }
    
    // Send message to server
    if (SSL_write(ssl, message, strlen(message)) <= 0) {
        print_ssl_error("Unable to write to server");
        goto cleanup_ssl;
    }
    
    // Read server response
    int bytes = SSL_read(ssl, buffer, sizeof(buffer) - 1);
    if (bytes > 0) {
        buffer[bytes] = '\0';
        printf("Received: %s\n", buffer);
        result = EXIT_SUCCESS;
    } else if (bytes == 0) {
        printf("Server closed connection\n");
    } else {
        print_ssl_error("Unable to read from server");
    }
    
cleanup_ssl:
    // Shutdown SSL connection
    SSL_shutdown(ssl);
    SSL_free(ssl);
    
cleanup_socket:
    close(sock);
    
cleanup_ctx:
    SSL_CTX_free(ctx);
    
cleanup_openssl:
    cleanup_openssl();
    
    return result;
}

/**
 * Print usage information
 */
void usage(const char *program_name) {
    printf("Usage: %s [OPTIONS] [MESSAGE]\n", program_name);
    printf("Options:\n");
    printf("  -h, --host HOST       Connect to HOST (default: localhost)\n");
    printf("  -p, --port PORT       Connect to PORT (default: 4433)\n");
    printf("  -c, --cafile FILE     Use FILE as CA certificate file\n");
    printf("  -d, --capath DIR      Use DIR as CA certificate directory\n");
    printf("  -n, --no-verify       Disable peer verification\n");
    printf("  -s, --skip-hostname   Skip hostname verification\n");
    printf("\nExample:\n");
    printf("  %s -h example.com -p 443 \"Hello, secure server!\"\n", program_name);
}

/**
 * Parse command line arguments
 */
int parse_args(int argc, char *argv[], tls_client_config *config, char **message) {
    int i;
    
    // Set default values
    config->host = DEFAULT_HOST;
    config->port = DEFAULT_PORT;
    config->ca_file = NULL;
    config->ca_path = NULL;
    config->verify_peer = 1;
    config->verify_hostname = 1;
    *message = "Hello, secure world!";
    
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--host") == 0) {
            if (++i < argc) {
                config->host = argv[i];
            } else {
                fprintf(stderr, "Error: Missing argument for %s\n", argv[i-1]);
                return 0;
            }
        } else if (strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--port") == 0) {
            if (++i < argc) {
                config->port = atoi(argv[i]);
                if (config->port <= 0 || config->port > 65535) {
                    fprintf(stderr, "Error: Invalid port number\n");
                    return 0;
                }
            } else {
                fprintf(stderr, "Error: Missing argument for %s\n", argv[i-1]);
                return 0;
            }
        } else if (strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--cafile") == 0) {
            if (++i < argc) {
                config->ca_file = argv[i];
            } else {
                fprintf(stderr, "Error: Missing argument for %s\n", argv[i-1]);
                return 0;
            }
        } else if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--capath") == 0) {
            if (++i < argc) {
                config->ca_path = argv[i];
            } else {
                fprintf(stderr, "Error: Missing argument for %s\n", argv[i-1]);
                return 0;
            }
        } else if (strcmp(argv[i], "-n") == 0 || strcmp(argv[i], "--no-verify") == 0) {
            config->verify_peer = 0;
        } else if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--skip-hostname") == 0) {
            config->verify_hostname = 0;
        } else if (strcmp(argv[i], "--help") == 0) {
            return 0;
        } else if (argv[i][0] == '-') {
            fprintf(stderr, "Error: Unknown option %s\n", argv[i]);
            return 0;
        } else {
            *message = argv[i];
        }
    }
    
    return 1;
}

/**
 * Main function
 */
int main(int argc, char *argv[]) {
    tls_client_config config;
    char *message;
    
    // Parse command line arguments
    if (!parse_args(argc, argv, &config, &message)) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }
    
    // Run TLS client
    return run_tls_client(&config, message);
}
```

#### Compilation and Testing
1. Save the code as `tls_client.c`
2. Compile:
   ```
   gcc -Wall -Wextra -o tls_client tls_client.c -lssl -lcrypto
   ```
3. Ensure the TLS server is running
4. Test with various options:
   ```
   # Connect to local server with default settings
   ./tls_client
   
   # Connect to specific host/port
   ./tls_client -h example.com -p 443
   
   # Disable verification (not recommended for production)
   ./tls_client -n
   ```

---

## Part 4: HTTPS Implementation

### Section 4.1: HTTPS Client with LibCurl

#### Objective
Create a client that can connect to HTTPS servers with proper certificate validation.

#### Requirements
- Use LibCurl to implement HTTPS requests
- Configure proper certificate verification
- Support both GET and POST methods
- Handle redirects and various status codes
- Format and display response headers and body

#### Implementation
```c
/**
 * https_client.c - HTTPS client implementation using LibCurl
 * 
 * This program demonstrates how to securely connect to HTTPS servers
 * with proper certificate validation.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

// Structure to store response data
struct ResponseData {
    char *data;
    size_t size;
};

// Structure to store header data
struct HeaderData {
    char *data;
    size_t size;
};

/**
 * Callback function for body data
 */
static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t real_size = size * nmemb;
    struct ResponseData *resp = (struct ResponseData *)userp;
    
    // Reallocate memory
    char *ptr = realloc(resp->data, resp->size + real_size + 1);
    if (!ptr) {
        fprintf(stderr, "Error: Not enough memory\n");
        return 0;
    }
    
    resp->data = ptr;
    memcpy(&(resp->data[resp->size]), contents, real_size);
    resp->size += real_size;
    resp->data[resp->size] = 0;
    
    return real_size;
}

/**
 * Callback function for header data
 */
static size_t header_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t real_size = size * nmemb;
    struct HeaderData *header = (struct HeaderData *)userp;
    
    // Reallocate memory
    char *ptr = realloc(header->data, header->size + real_size + 1);
    if (!ptr) {
        fprintf(stderr, "Error: Not enough memory for headers\n");
        return 0;
    }
    
    header->data = ptr;
    memcpy(&(header->data[header->size]), contents, real_size);
    header->size += real_size;
    header->data[header->size] = 0;
    
    return real_size;
}

/**
 * Initialize response data structure
 */
static void init_response_data(struct ResponseData *data) {
    data->data = malloc(1);
    if (!data->data) {
        fprintf(stderr, "Error: Failed to allocate memory\n");
        exit(EXIT_FAILURE);
    }
    data->size = 0;
    data->data[0] = '\0';
}

/**
 * Initialize header data structure
 */
static void init_header_data(struct HeaderData *data) {
    data->data = malloc(1);
    if (!data->data) {
        fprintf(stderr, "Error: Failed to allocate memory\n");
        exit(EXIT_FAILURE);
    }
    data->size = 0;
    data->data[0] = '\0';
}

/**
 * Perform HTTPS POST request with JSON data
 */
int https_post_json(const char *url, const char *json_data, int verbose, int insecure) {
    CURL *curl;
    CURLcode res;
    struct ResponseData response;
    struct HeaderData headers;
    struct curl_slist *header_list = NULL;
    
    // Initialize data structures
    init_response_data(&response);
    init_header_data(&headers);
    
    // Initialize curl
    curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "Error: Failed to initialize curl\n");
        free(response.data);
        free(headers.data);
        return EXIT_FAILURE;
    }
    
    // Add JSON headers
    header_list = curl_slist_append(header_list, "Content-Type: application/json");
    header_list = curl_slist_append(header_list, "Accept: application/json");
    
    // Set options
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_data);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&response);
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, header_callback);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, (void *)&headers);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "curl/7.68.0");
    
    // Set verbose mode if requested
    if (verbose) {
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    }
    
    // Set insecure mode if requested (not recommended for production)
    if (insecure) {
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        fprintf(stderr, "Warning: SSL certificate verification disabled\n");
    }
    
    // Perform the request
    res = curl_easy_perform(curl);
    
    // Check for errors
    if (res != CURLE_OK) {
        fprintf(stderr, "HTTPS request failed: %s\n", curl_easy_strerror(res));
        free(response.data);
        free(headers.data);
        curl_slist_free_all(header_list);
        curl_easy_cleanup(curl);
        return EXIT_FAILURE;
    }
    
    // Get response information
    long status_code;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status_code);
    
    // Print response details
    printf("Status code: %ld\n", status_code);
    printf("\n--- Response Headers ---\n%s", headers.data);
    
    // Parse content type (simplified)
    char *content_type = NULL;
    curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &content_type);
    
    if (content_type) {
        printf("\n--- Response Body (%s) ---\n", content_type);
    } else {
        printf("\n--- Response Body ---\n");
    }
    
    // Print response body (limit output for large responses)
    if (response.size > 1024 && !verbose) {
        printf("%.*s\n[...]\n(Response truncated, use --verbose to see full response)\n", 
               1024, response.data);
    } else {
        printf("%s\n", response.data);
    }
    
    // Clean up
    free(response.data);
    free(headers.data);
    curl_slist_free_all(header_list);
    curl_easy_cleanup(curl);
    
    return EXIT_SUCCESS;
}

/**
 * Print usage information
 */
void usage(const char *program_name) {
    printf("Usage: %s [OPTIONS] <URL>\n", program_name);
    printf("Options:\n");
    printf("  -X, --request METHOD   Specify request method (GET or POST)\n");
    printf("  -d, --data DATA        HTTP POST data\n");
    printf("  -v, --verbose          Make the operation more verbose\n");
    printf("  -k, --insecure         Allow insecure server connections\n");
    printf("  -h, --help             Display this help message\n");
    printf("\nExamples:\n");
    printf("  %s https://example.com\n", program_name);
    printf("  %s -v https://api.example.com/status\n", program_name);
    printf("  %s -X POST -d '{\"key\":\"value\"}' https://api.example.com/data\n", program_name);
}

/**
 * Main function
 */
int main(int argc, char *argv[]) {
    int i;
    const char *url = NULL;
    const char *method = "GET";
    const char *data = NULL;
    int verbose = 0;
    int insecure = 0;
    
    // Initialize curl globally
    curl_global_init(CURL_GLOBAL_DEFAULT);
    
    // Parse command line arguments
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-X") == 0 || strcmp(argv[i], "--request") == 0) {
            if (++i < argc) {
                method = argv[i];
            } else {
                fprintf(stderr, "Error: Missing argument for %s\n", argv[i-1]);
                usage(argv[0]);
                curl_global_cleanup();
                return EXIT_FAILURE;
            }
        } else if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--data") == 0) {
            if (++i < argc) {
                data = argv[i];
            } else {
                fprintf(stderr, "Error: Missing argument for %s\n", argv[i-1]);
                usage(argv[0]);
                curl_global_cleanup();
                return EXIT_FAILURE;
            }
        } else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
            verbose = 1;
        } else if (strcmp(argv[i], "-k") == 0 || strcmp(argv[i], "--insecure") == 0) {
            insecure = 1;
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            usage(argv[0]);
            curl_global_cleanup();
            return EXIT_SUCCESS;
        } else if (argv[i][0] == '-') {
            fprintf(stderr, "Error: Unknown option %s\n", argv[i]);
            usage(argv[0]);
            curl_global_cleanup();
            return EXIT_FAILURE;
        } else {
            url = argv[i];
        }
    }
    
    // Check if URL is provided
    if (!url) {
        fprintf(stderr, "Error: URL is required\n");
        usage(argv[0]);
        curl_global_cleanup();
        return EXIT_FAILURE;
    }
    
    int result;
    if (strcmp(method, "GET") == 0) {
        result = https_get(url, verbose, insecure);
    } else if (strcmp(method, "POST") == 0) {
        if (!data) {
            fprintf(stderr, "Error: POST request requires data\n");
            usage(argv[0]);
            curl_global_cleanup();
            return EXIT_FAILURE;
        }
        result = https_post_json(url, data, verbose, insecure);
    } else {
        fprintf(stderr, "Error: Unsupported method: %s\n", method);
        usage(argv[0]);
        curl_global_cleanup();
        return EXIT_FAILURE;
    }
    
    curl_global_cleanup();
    return result;
}

/**
 * Perform HTTPS GET request
 */
int https_get(const char *url, int verbose, int insecure) {
    CURL *curl;
    CURLcode res;
    struct ResponseData response;
    struct HeaderData headers;
    
    // Initialize data structures
    init_response_data(&response);
    init_header_data(&headers);
    
    // Initialize curl
    curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "Error: Failed to initialize curl\n");
        free(response.data);
        free(headers.data);
        return EXIT_FAILURE;
    }
    
    // Set options
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&response);
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, header_callback);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, (void *)&headers);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "curl/7.68.0");
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 5L);
    
    // Set verbose mode if requested
    if (verbose) {
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    }
    
    // Set insecure mode if requested (not recommended for production)
    if (insecure) {
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        fprintf(stderr, "Warning: SSL certificate verification disabled\n");
    }
    
    // Perform the request
    res = curl_easy_perform(curl);
    
    // Check for errors
    if (res != CURLE_OK) {
        fprintf(stderr, "HTTPS request failed: %s\n", curl_easy_strerror(res));
        free(response.data);
        free(headers.data);
        curl_easy_cleanup(curl);
        return EXIT_FAILURE;
    }
    
    // Get response information
    long status_code;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status_code);
    
    // Print response details
    printf("Status code: %ld\n", status_code);
    printf("\n--- Response Headers ---\n%s", headers.data);
    
    // Parse content type (simplified)
    char *content_type = NULL;
    curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &content_type);
    
    if (content_type) {
        printf("\n--- Response Body (%s) ---\n", content_type);
    } else {
        printf("\n--- Response Body ---\n");
    }
    
    // Print response body (limit output for large responses)
    if (response.size > 1024 && !verbose) {
        printf("%.*s\n[...]\n(Response truncated, use --verbose to see full response)\n", 
               1024, response.data);
    } else {
        printf("%s\n", response.data);
    }
    
    // Clean up
    free(response.data);
    free(headers.data);
    curl_easy_cleanup(curl);
    
    return EXIT_SUCCESS;
} tcp_client.c - A simple TCP client for connecting to network services
 * 
 * This program creates a TCP socket connection to a specified server and port,
 * then forwards stdin to the server and prints all responses.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>

#define BUFFER_SIZE 1024

// Global socket descriptor for signal handler
int global_sock = -1;

/**
 * Signal handler for graceful termination
 */
void handle_signal(int sig) {
    if (global_sock != -1) {
        printf("\nClosing connection...\n");
        close(global_sock);
    }
    exit(EXIT_SUCCESS);
}

/**
 * Validate command line arguments
 */
void validate_args(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <server_ip> <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    // Validate port number
    char *endptr;
    long port = strtol(argv[2], &endptr, 10);
    if (*endptr != '\0' || port <= 0 || port > 65535) {
        fprintf(stderr, "Error: Invalid port number. Must be between 1-65535.\n");
        exit(EXIT_FAILURE);
    }
}

/**
 * Create and configure a socket
 */
int create_socket() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }
    
    // Set socket options if needed
    int flag = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag)) < 0) {
        perror("setsockopt failed");
        close(sock);
        exit(EXIT_FAILURE);
    }
    
    return sock;
}

/**
 * Connect to server
 */
void connect_to_server(int sock, const char *server_ip, int port) {
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    
    // Convert IP address from text to binary form
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        perror("Invalid address or address not supported");
        close(sock);
        exit(EXIT_FAILURE);
    }
    
    // Connect to server
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(sock);
        exit(EXIT_FAILURE);
    }
    
    printf("Connected to %s:%d\n", server_ip, port);
}

/**
 * Main client function
 */
int main(int argc, char *argv[]) {
    validate_args(argc, argv);
    
    const char *server_ip = argv[1];
    int port = atoi(argv[2]);
    
    // Set up signal handler for clean termination
    signal(SIGINT, handle_signal);
    
    // Create socket
    int sock = create_socket();
    global_sock = sock;  // Store for signal handler
    
    // Connect to server
    connect_to_server(sock, server_ip, port);
    
    // Buffer for data
    char buffer[BUFFER_SIZE];
    
    printf("Type messages to send (Ctrl+C to quit):\n");
    
    // Main communication loop
    while (1) {
        // Read input from user
        printf("> ");
        if (fgets(buffer, BUFFER_SIZE, stdin) == NULL) {
            break;  // End of input
        }
        
        // Send data to server
        size_t len = strlen(buffer);
        ssize_t bytes_sent = send(sock, buffer, len, 0);
        if (bytes_sent < 0) {
            perror("Error sending data");
            break;
        } else if (bytes_sent < len) {
            fprintf(stderr, "Warning: Only sent %zd of %zu bytes\n", bytes_sent, len);
        }
        
        // Receive response from server
        ssize_t bytes_received = recv(sock, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_received < 0) {
            perror("Error receiving data");
            break;
        } else if (bytes_received == 0) {
            printf("Server closed connection\n");
            break;
        }
        
        // Null-terminate and display the response
        buffer[bytes_received] = '\0';
        printf("Server: %s", buffer);
    }
    
    // Clean up
    close(sock);
    printf("Connection closed\n");
    
    return 0;
}
```

#### Compilation and Testing
1. Save the code as `tcp_client.c`
2. Compile with:
   ```
   gcc -Wall -Wextra -o tcp_client tcp_client.c
   ```
3. Run the client to connect to the echo server:
   ```
   ./tcp_client 127.0.0.1 65432
   ```
4. Type messages to observe the echo response

---

## Part 2: HTTP Implementation

### Section 2.1: Flask HTTP Server

#### Objective
Develop a HTTP server using Flask that implements multiple endpoints with appropriate response handling.

#### Requirements
- Create a Flask application with at least three endpoints:
  - `/` - Returns HTML welcome page
  - `/api/status` - Returns JSON server status information
  - `/api/echo` - Returns data sent in the request
- Implement proper HTTP status codes and headers
- Handle different HTTP methods (GET, POST)
- Document API endpoints with appropriate comments

#### Implementation
```python
#!/usr/bin/env python3
"""
Flask HTTP Server Implementation
This program demonstrates a simple HTTP server with multiple endpoints and
different response types.
"""
import platform
import datetime
from flask import Flask, request, jsonify, render_template_string

app = Flask(__name__)

# Simple HTML template for the root endpoint
HTML_TEMPLATE = """
<!DOCTYPE html>
<html>
<head>
    <title>HTTP Server Demo</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            max-width: 800px;
            margin: 0 auto;
            padding: 20px;
            line-height: 1.6;
        }
        h1 {
            color: #333;
        }
        .endpoint {
            background-color: #f5f5f5;
            border-left: 4px solid #007bff;
            padding: 10px;
            margin-bottom: 10px;
        }
        code {
            background-color: #eee;
            padding: 2px 4px;
            border-radius: 4px;
        }
    </style>
</head>
<body>
    <h1>HTTP Server Demo</h1>
    <p>This server demonstrates a simple HTTP implementation with the following endpoints:</p>
    
    <div class="endpoint">
        <h3>GET /</h3>
        <p>The page you're currently viewing.</p>
    </div>
    
    <div class="endpoint">
        <h3>GET /api/status</h3>
        <p>Returns server status information as JSON.</p>
        <p>Try it: <a href="/api/status">/api/status</a></p>
    </div>
    
    <div class="endpoint">
        <h3>POST /api/echo</h3>
        <p>Echoes back the JSON or form data sent in the request.</p>
        <p>Example using curl:</p>
        <code>curl -X POST -H "Content-Type: application/json" -d '{"message":"Hello"}' http://localhost:5000/api/echo</code>
    </div>
</body>
</html>
"""

@app.route('/')
def index():
    """Serve the index page with API documentation."""
    return render_template_string(HTML_TEMPLATE)

@app.route('/api/status')
def status():
    """Return server status information as JSON."""
    status_info = {
        "status": "operational",
        "timestamp": datetime.datetime.now().isoformat(),
        "server_info": {
            "platform": platform.platform(),
            "python_version": platform.python_version(),
            "implementation": platform.python_implementation()
        },
        "endpoints": ["/", "/api/status", "/api/echo"]
    }
    return jsonify(status_info)

@app.route('/api/echo', methods=['POST'])
def echo():
    """Echo back the request data.
    
    Accepts either JSON or form data and returns it in the response.
    """
    if request.is_json:
        return jsonify(request.json)
    elif request.form:
        return jsonify(dict(request.form))
    else:
        return jsonify({"error": "No data provided"}), 400

@app.errorhandler(404)
def not_found(e):
    """Handle 404 errors."""
    return jsonify({"error": "Resource not found"}), 404

@app.errorhandler(500)
def server_error(e):
    """Handle 500 errors."""
    return jsonify({"error": "Internal server error"}), 500

if __name__ == '__main__':
    app.run(host='127.0.0.1', port=5000, debug=True)
```

#### Testing Instructions
1. Save the code as `flask_server.py`
2. Install Flask if needed: `pip install flask`
3. Run the server: `python flask_server.py`
4. Test the endpoints:
   - Browse to http://127.0.0.1:5000/ to see the HTML page
   - Browse to http://127.0.0.1:5000/api/status to see the JSON status
   - Use curl to test the echo endpoint:
     ```
     curl -X POST -H "Content-Type: application/json" -d '{"message":"Hello"}' http://127.0.0.1:5000/api/echo
     ```

---

### Section 2.2: HTTP Client (C)

#### Objective
Implement a C program that acts as an HTTP client, capable of sending requests and processing responses.

#### Requirements
- Develop a client that can send HTTP requests to the Flask server
- Implement GET and POST methods
- Parse HTTP responses correctly
- Handle headers and status codes
- Manage connection properly

#### Implementation
```c
/**
 * http_client.c - An HTTP client implementation in C
 * 
 * This program demonstrates how to create HTTP requests and parse HTTP responses
 * using standard socket programming.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>

#define BUFFER_SIZE 4096
#define HTTP_PORT 5000
#define SERVER_ADDR "127.0.0.1"

/**
 * Send a GET request to the specified path
 */
int http_get(const char *path) {
    int sock;
    struct sockaddr_in server_addr;
    char request[BUFFER_SIZE];
    char response[BUFFER_SIZE];
    ssize_t bytes_received;
    
    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        return -1;
    }
    
    // Prepare server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(HTTP_PORT);
    
    if (inet_pton(AF_INET, SERVER_ADDR, &server_addr.sin_addr) <= 0) {
        perror("Invalid address");
        close(sock);
        return -1;
    }
    
    // Connect to server
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(sock);
        return -1;
    }
    
    // Format HTTP GET request
    snprintf(request, BUFFER_SIZE,
             "GET %s HTTP/1.1\r\n"
             "Host: %s:%d\r\n"
             "Connection: close\r\n"
             "User-Agent: C-HTTP-Client/1.0\r\n"
             "\r\n",
             path, SERVER_ADDR, HTTP_PORT);
    
    // Send the request
    if (send(sock, request, strlen(request), 0) < 0) {
        perror("Send failed");
        close(sock);
        return -1;
    }
    
    printf("HTTP Request Sent:\n%s\n", request);
    printf("Response:\n");
    
    // Receive the response
    int total_bytes = 0;
    while ((bytes_received = recv(sock, response, BUFFER_SIZE - 1, 0)) > 0) {
        response[bytes_received] = '\0';
        printf("%s", response);
        total_bytes += bytes_received;
    }
    
    if (bytes_received < 0) {
        perror("Receive failed");
    }
    
    printf("\nTotal bytes received: %d\n", total_bytes);
    
    close(sock);
    return 0;
}

/**
 * Send a POST request with JSON data to the specified path
 */
int http_post_json(const char *path, const char *json_data) {
    int sock;
    struct sockaddr_in server_addr;
    char request[BUFFER_SIZE];
    char response[BUFFER_SIZE];
    ssize_t bytes_received;
    
    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        return -1;
    }
    
    // Prepare server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(HTTP_PORT);
    
    if (inet_pton(AF_INET, SERVER_ADDR, &server_addr.sin_addr) <= 0) {
        perror("Invalid address");
        close(sock);
        return -1;
    }
    
    // Connect to server
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(sock);
        return -1;
    }
    
    // Format HTTP POST request with JSON data
    snprintf(request, BUFFER_SIZE,
             "POST %s HTTP/1.1\r\n"
             "Host: %s:%d\r\n"
             "Connection: close\r\n"
             "User-Agent: C-HTTP-Client/1.0\r\n"
             "Content-Type: application/json\r\n"
             "Content-Length: %zu\r\n"
             "\r\n"
             "%s",
             path, SERVER_ADDR, HTTP_PORT, strlen(json_data), json_data);
    
    // Send the request
    if (send(sock, request, strlen(request), 0) < 0) {
        perror("Send failed");
        close(sock);
        return -1;
    }
    
    printf("HTTP Request Sent:\n%s\n", request);
    printf("Response:\n");
    
    // Receive the response
    int total_bytes = 0;
    while ((bytes_received = recv(sock, response, BUFFER_SIZE - 1, 0)) > 0) {
        response[bytes_received] = '\0';
        printf("%s", response);
        total_bytes += bytes_received;
    }
    
    if (bytes_received < 0) {
        perror("Receive failed");
    }
    
    printf("\nTotal bytes received: %d\n", total_bytes);
    
    close(sock);
    return 0;
}

/**
 * Parse and display command line arguments
 */
void usage(const char *program_name) {
    printf("Usage: %s [OPTIONS]\n", program_name);
    printf("Options:\n");
    printf("  get PATH           Send HTTP GET request to PATH\n");
    printf("  post PATH JSON     Send HTTP POST with JSON data to PATH\n");
    printf("\nExamples:\n");
    printf("  %s get /api/status\n", program_name);
    printf("  %s post /api/echo '{\"message\":\"hello\"}'\n", program_name);
}

/**
 * Main program function
 */
int main(int argc, char *argv[]) {
    if (argc < 3) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }
    
    if (strcmp(argv[1], "get") == 0) {
        return http_get(argv[2]) == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
    } else if (strcmp(argv[1], "post") == 0) {
        if (argc < 4) {
            printf("Error: POST requires a JSON data parameter\n");
            usage(argv[0]);
            return EXIT_FAILURE;
        }
        return http_post_json(argv[2], argv[3]) == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
    } else {
        printf("Unknown command: %s\n", argv[1]);
        usage(argv[0]);
        return EXIT_FAILURE;
    }
}
```

#### Compilation and Testing
1. Save the code as `http_client.c`
2. Compile:
   ```
   gcc -Wall -Wextra -o http_client http_client.c
   ```
3. Ensure the Flask server is running
4. Test with:
   ```
   ./http_client get /api/status
   ./http_client post /api/echo '{"message":"hello world"}'
   ```

---

### Section 2.3: HTTP Client with LibCurl

#### Objective
Implement an HTTP client using the LibCurl library to demonstrate a more robust approach to HTTP communications.

#### Requirements
- Use the LibCurl library to create an HTTP client
- Implement GET and POST functionality
- Support custom headers
- Handle response data with callback functions
- Provide verbose debugging output

#### Implementation
```c
/**
 * libcurl_client.c - HTTP client implementation using LibCurl
 * 
 * This program demonstrates how to use the LibCurl library to perform
 * HTTP requests with clean error handling and data processing.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

// Server configuration
#define SERVER_URL "http://127.0.0.1:5000"

/**
 * Structure to store response data
 */
struct ResponseData {
    char *data;
    size_t size;
};

/**
 * Callback function to handle received data
 */
static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t real_size = size * nmemb;
    struct ResponseData *resp = (struct ResponseData *)userp;
    
    // Reallocate memory for the response data
    char *ptr = realloc(resp->data, resp->size + real_size + 1);
    if (!ptr) {
        fprintf(stderr, "Error: Not enough memory\n");
        return 0;  // Signals error to curl
    }
    
    resp->data = ptr;
    memcpy(&(resp->data[resp->size]), contents, real_size);
    resp->size += real_size;
    resp->data[resp->size] = 0;  // Null-terminate
    
    return real_size;
}

/**
 * Perform HTTP GET request
 */
int perform_http_get(const char *endpoint) {
    CURL *curl;
    CURLcode res;
    struct ResponseData response_data = {0};
    char url[512];
    
    // Initialize response data
    response_data.data = malloc(1);
    if (!response_data.data) {
        fprintf(stderr, "Error: Failed to allocate memory\n");
        return -1;
    }
    response_data.size = 0;
    response_data.data[0] = '\0';
    
    // Construct full URL
    snprintf(url, sizeof(url), "%s%s", SERVER_URL, endpoint);
    
    // Initialize curl
    curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "Error: Failed to initialize curl\n");
        free(response_data.data);
        return -1;
    }
    
    // Set curl options
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&response_data);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-client/1.0");
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);  // Enable verbose for debugging
    
    // Perform the request
    res = curl_easy_perform(curl);
    
    // Check for errors
    if (res != CURLE_OK) {
        fprintf(stderr, "Error: curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    } else {
        // Get status code
        long status_code;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status_code);
        
        // Print response information
        printf("Status Code: %ld\n", status_code);
        printf("Response Size: %zu bytes\n", response_data.size);
        printf("Response Body:\n%s\n", response_data.data);
    }
    
    // Clean up
    curl_easy_cleanup(curl);
    free(response_data.data);
    
    return (res == CURLE_OK) ? 0 : -1;
}

/**
 * Perform HTTP POST request with JSON data
 */
int perform_http_post_json(const char *endpoint, const char *json_data) {
    CURL *curl;
    CURLcode res;
    struct ResponseData response_data = {0};
    char url[512];
    struct curl_slist *headers = NULL;
    
    // Initialize response data
    response_data.data = malloc(1);
    if (!response_data.data) {
        fprintf(stderr, "Error: Failed to allocate memory\n");
        return -1;
    }
    response_data.size = 0;
    response_data.data[0] = '\0';
    
    // Construct full URL
    snprintf(url, sizeof(url), "%s%s", SERVER_URL, endpoint);
    
    // Initialize curl
    curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "Error: Failed to initialize curl\n");
        free(response_data.data);
        return -1;
    }
    
    // Set headers
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "Accept: application/json");
    
    // Set curl options
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_data);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&response_data);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-client/1.0");
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);  // Enable verbose for debugging
    
    // Perform the request
    res = curl_easy_perform(curl);
    
    // Check for errors
    if (res != CURLE_OK) {
        fprintf(stderr, "Error: curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    } else {
        // Get status code
        long status_code;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status_code);
        
        // Print response information
        printf("Status Code: %ld\n", status_code);
        printf("Response Size: %zu bytes\n", response_data.size);
        printf("Response Body:\n%s\n", response_data.data);
    }
    
    // Clean up
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    free(response_data.data);
    
    return (res == CURLE_OK) ? 0 : -1;
}

/**
 * Print usage information
 */
void usage(const char *program_name) {
    printf("Usage: %s [OPTIONS]\n", program_name);
    printf("Options:\n");
    printf("  get ENDPOINT          Send HTTP GET request to ENDPOINT\n");
    printf("  post ENDPOINT JSON    Send HTTP POST with JSON data to ENDPOINT\n");
    printf("\nExamples:\n");
    printf("  %s get /api/status\n", program_name);
    printf("  %s post /api/echo '{\"message\":\"hello\"}'\n", program_name);
}

/**
 