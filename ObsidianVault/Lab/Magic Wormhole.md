

Magic-Wormhole allows users to securely transfer files or directories between computers. By sharing a short, human-readable code, two devices can establish a secure connection to exchange data.
**Official Documentation:** [Magic-Wormhole Documentation](https://magic-wormhole.readthedocs.io/en/latest/)

## TLDR
Use magic wormhole to send files from any 2 machines. Works for LAN and WAN

```
$ sudo apt-get install magic-wormhole
# or 
$ pipx install magic-wormhole


# user 
$ wormhole send file.txt
Wormhole code is: 6-fascinate-tapeworm
On the other computer, please run:

wormhole receive 6-fascinate-tapeworm

# User B 
$ wormhole receive 6-fascinate-tapeworm
```

### PAKE (Password-Authenticated Key Exchange)

PAKE is a cryptographic protocol that enables two parties to establish a secure communication channel based on a shared password, without revealing the password itself during the exchange. This approach ensures that even if an attacker intercepts the communication, they cannot deduce the password or the resulting encryption keys. In the context of Magic-Wormhole, the shared "wormhole code" acts as the password, facilitating a secure key exchange between the sender and receiver.  The protocol is setup where an attacker gets one guess for both the channel id, and the password used in the channel.

Magic-Wormhole uses two primary servers to facilitate communication between clients:

1. **Mailbox (Rendezvous) Server**: A WebSocket-based relay that delivers small key-exchange and control messages from one client to another. This allows the wormhole codes to omit IP addresses and port numbers. Clients connect with WebSockets for low-latency delivery when both clients are online simultaneously. 

2. **Transit Relay Server**: Assists clients in establishing bulk-data transit connections, especially when both are behind NAT boxes. Each side makes a TCP connection to this server and presents a handshake. Two connections with identical handshakes are glued together, allowing them to pretend they have a direct connection. The transit relay now supports WebSockets, enabling browser-based clients to use it as well. 

Below is a simplified representation of how Magic-Wormhole facilitates a file transfer:

```
+-----------+       Wormhole Code       +-----------+
|  Sender   |  ---------------------->  | Receiver  |
|           |                           |           |
|  (Device) |  <----------------------  | (Device)  |
+-----------+     Secure Connection     +-----------+
       |                                     ^
       v                                     |
+---------------------------------------------------+
|                  Relay Servers                   |
|                                                  |
|  +-------------------+       +----------------+  |
|  | Mailbox Server    | <---> | Transit Relay  |  |
|  | (WebSocket-based) |       | (TCP/WebSocket)|  |
|  +-------------------+       +----------------+  |
+---------------------------------------------------+
```

###  Sending a File from One Machine to Another

**Prerequisites:**

- Ensure Magic-Wormhole is installed on both the sender's and receiver's machines.
  - **Installation Instructions:** [Magic-Wormhole Installation](https://magic-wormhole.readthedocs.io/en/latest/welcome.html)

**Steps:**

1. **Sender:**
   - Open a terminal.
   - Navigate to the directory containing the file to be sent.
   - Execute the command:
     ```bash
     wormhole send filename.txt
     ```
   - The terminal will display a wormhole code, e.g., `7-crossover-clockwork`.
   - Share this code with the receiver through a secure channel (e.g., in person, encrypted message).

2. **Receiver:**
   - Open a terminal.
   - Execute the command:
     ```bash
     wormhole receive
     ```
   - When prompted, enter the wormhole code provided by the sender. Note you can use tab complete for most of the code words. you can also increase the size of the code if you are feeling paranoid.
   - The terminal will display details about the incoming file, including its name and size.
   - Confirm the transfer by typing `y` (yes) or `n` (no).
   - Upon confirmation, the file will be securely downloaded to the current directory.

**Note:** Both the sender and receiver should keep their terminals open until the transfer is complete to maintain the connection.
