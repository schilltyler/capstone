


---

<!-- slide template="[[Title Slide]]" -->
## Capstone & C2 Engineering

---

<!-- slide template="[[Base Slide]]" -->
::: title
#### Academic Integrity and Mental Health 
:::

- TL;DR:
  - I give incompletes.
  - I am not a cop.
  - I give extensions.
  - If you tell me you need an incomplete, I will grant one—no questions asked. (You don’t need to explain your situation if you’re low on emotional energy.)

---

<!-- slide template="[[Base Slide]]" -->
::: title
#### Capstone: Developing Your Own C2 
:::

- To pass the class, your team must build a custom an implant and listening post 
- Work in groups of up to three people. (Infosec is a team sport—collaborate and make friends!)
- **Do not work alone.**
- Use a private git repository (GitHub or GitLab) and add my account (`kbsec`) so I can track contributions.
- Start early—every assignment builds toward your capstone.

---

<!-- slide template="[[Base Slide]]" -->
::: title
#### Groups
:::

- Form as large a team as feasible; the workload is too heavy for one person.
- Your group will have defined milestones to meet full credit.
- This is a prime opportunity to to show off your skills to folks who will attend the poster session 

---

<!-- slide template="[[Base Slide]]" -->
::: title
#### Milestones 
:::
.
- all work will be submitted to a combination of gradescope and github 
- **2/17:** Pick your teammates, and pick a team name.
- **2/20:** Final deadline for scheduling your  roadmap meeting.
- **2/22:** Complete execution phase
- **03/03**  Lock in your special feature description
- **03/03** Complete basic file IO functionality
- **03/10**: classes resume 
- **03/15** Complete TCP/TLS channel 
- **03/22** Complete HTTP/HTTPS channel
- **03/29** Complete loader functionality
- **04/01** The target environment is released 
- **04/01** Complete reflective payload 
- **04/01** Complete injection feature 
- **04/01** Complete PLT hooking feature 
- **04/01** at this point you should be testing your implant
- **04/22:** Final presentation and poster session: 6-9 pm est.
- **04/25:** Final write up submission deadline. you should submit before this date but this is the latest I can support 

---

<!-- slide template="[[Base Slide]]" -->
::: title
#### Terminology: Review
:::

- **C2:** Command and Control Server.
- **Implant:** The malware component deployed on a target machine.
- **Client:** The interface or software that operators use to control implants via the C2 server.

---

<!-- slide template="[[Base Slide]]" -->
::: title
#### Sections for the Capstone
:::

- Implant
- C2 Server
- Client 
- Poster
- Writeup  

---

<!-- slide template="[[Base Slide]]" -->
::: title
#### Requirements: Implant Functionality 
:::

- You must develop an implant for **aarch64 Linux**.
- The implant must meet a variety of requirements to earn full credit—refer to the rubric for details.

---

<!-- slide template="[[Base Slide]]" -->
::: title
#### Implant Functionality Groups 
:::

- RPC and C2 Channel
- Cryptography
- Situational Awareness
- Execution & Injection
- File I/O
- Persistence 
- Loot
- Defense Evasion 

---

<!-- slide template="[[Base Slide]]" -->
::: title
#### Implant → RPC 
:::

- Your implant should communicate with the C2 server using a Remote Procedure Call (RPC) framework.
- Build the RPC on top of an approved C2 channel and structured data format (e.g., JSON, TLV, SOAP, Protocol Buffers).

---

<!-- slide template="[[Base Slide]]" -->
::: title
#### Implant → C2 Channel: RPC Tasking
:::

- The RPC  should be asynchronous.
- Operators issue tasks via the client.
- The implant checks in with the server, downloads tasks, executes them, and then sends back results.
- the implant needs to support a low latency and a high latency channel
	- TCP/HTTP
- **Start with TCP/TLS.**



---

<!-- slide template="[[Base Slide]]" -->
::: title
#### Implant → C2 Channel: TCP 
:::

- The C2 server must handle both incoming data from implants and outgoing commands to clients.
- the connection is persistent until eithe the server  terminate the connection.
- Then after $n$ seconds, the implant checks back in.  If the server wants to do anything, the connections upgraded and messages are exchanged 


---

<!-- slide template="[[Base Slide]]" -->
::: title
#### Implant → C2 Channel: HTTP 
:::

- The C2 server must handle both incoming data from implants and outgoing commands to clients.
- all messages are exchanged in round trips 

---

<!-- slide template="[[Split Vertical]]" -->
::: title
#### Example C2 Architecture 
:::

::: left
- The infected aarch64 Linux machine connects directly to the C2.
- Operators issue commands via the client.
- the client sends data to the c2 server
- the c2 server sends the commands to the implant
- the implant performs the task, then sends the result to the c2. 
- client retrieves the result from the c2 
:::
::: right
![[0x0c_slide_14_image.png]]
:::

---

<!-- slide template="[[Split Vertical]]" -->
::: title
#### C2 Channel: HTTP
:::

::: left
- Example: an HTTP GET request from the implant.
:::
::: right
![[0x0c_slide_15_image.png|300]]
:::

---

<!-- slide template="[[Base Slide]]" -->
::: title
#### HTTP RPC
:::

- **RPC (Remote Procedure Call):** The protocol by which the C2 server sends commands (such as executing commands or transferring files) to the implant.
- A simple example: the implant maintains a backdoor and executes shell commands on Linux.

---

<!-- slide template="[[Base Slide]]" -->
::: title
#### Example 1: HTTP Reverse Shell 
:::

- The implant makes an HTTP GET request to the `/commands` endpoint.
- The server responds with shell commands to execute.
- The implant executes these commands and returns the output via an HTTP POST request.

---

<!-- slide template="[[Base Slide]]" -->
::: title
#### Example 1: HTTP Reverse Shell 
:::
<center>
![[0x0c_slide_18_image.png|400]]
</center>

---

<!-- slide template="[[Base Slide]]" -->
::: title
#### Channel Comments 
:::

- Use TLS (HTTPS) for secure communications.
- The implant should handle network interruptions gracefully.
- Although multithreading is not required, consider it for real‑world robustness.
- Note: Even with TLS, defenders might use techniques such as certificate pinning


---
<!-- slide template="[[Base Slide]]" -->
::: title
#### Channel: Config Modification
:::

- Each implant is compiled with configuration data including:
  - The C2 server addresses.
  - The sleep/dormancy interval and random jitter.
  - A kill date.
  - The server’s public key or some other pre shared secret.
- At runtime, allow modifications to some configuration items (persistence is not required).

---

<!-- slide template="[[Base Slide]]" -->
::: title
#### Implant → Cryptography
:::

- Your implant must use secure cryptography.
- **Building Blocks:**
  - Asymmetric cryptography (public‑key operations)
  - Symmetric cryptography (ciphers and MACs)
  - Hash functions
- **Features:**
  - **Session Establishment:** Securely connect and authenticate with the C2 server.
  - **Session Update:** Re‑establish the secure connection as needed.

---

<!-- slide template="[[Base Slide]]" -->
::: title
#### Implant → Cryptography (Asymmetric)
:::

- Use public‑key cryptography to securely establish a session with the C2.
- Options include performing a key exchange (e.g., Diffie‑Hellman) or embedding the C2’s public key in the implant to encrypt a session key.

---

<!-- slide template="[[Base Slide]]" -->
::: title
#### Implant → Cryptography (Symmetric)
:::

- Encrypt all sensitive configuration data and strings. Even a simple XOR cipher is acceptable if implemented creatively.
- Use authenticated encryption (e.g., AES‑GCM, chacha20 poly1305) when transmitting over a channel.

---

<!-- slide template="[[Base Slide]]" -->
::: title
#### Cryptography Comments 
:::

- **Do not roll your own crypto.** Use established libraries.
- **Recommended Libraries:** Libsodium, Monocypher, mbedtls, bearssl, LibreSSL.
- **Other Options:** OpenSSL (if you’re comfortable with its interface).
- Ensure your chosen library works in your build environment (and can be interfaced with Python / the language you use for the c2 server )
- Use your distro’s package manager or CMake‑friendly repositories when possible.
- I will give you 1% bonus points for uisng a nix flake :)
---

<!-- slide template="[[Base Slide]]" -->
::: title
#### Implant → Situational Awareness
:::

- The implant should be able to:
  - Read environment variables.
  - List network interfaces (IP addresses, MAC addresses, and interface names).
  - Determine the Linux distribution and kernel version.
  - Get the current username.
  - Retrieve the hostname.
  - Read the system’s machine ID (e.g., from `/etc/machine-id`).
  - List and change directories.
  - Enumerate running processes.
  - **Bonus:** Implement anti‑sandbox or anti‑virtualization checks
  - keep child processes to a minimum. i.e., don't spawn a child process to cat a file if you dont need to.
---

<!-- slide template="[[Base Slide]]" -->
::: title
#### Implant → Situational Awareness (Multiple Instances)
:::

- Ensure that multiple instances of your implant do not run concurrently on the same host.
  - Use lock files (e.g., via `flock`), PID files, or similar mechanisms.
- Consider limiting active instances (for example, one for interactive use, one for recovery, and one for long‑term operations).

---

<!-- slide template="[[Base Slide]]" -->
::: title
#### Implant → Execution & Injection
:::

- **Execution:**  
  - Spawn new processes with controlled input/output redirection.
  - Execute shellcode in a local process.
  - execute shell code in a child process
  - execute shell code in an arbitrary process
- **Injection:**  
  - Support code injection into another process (e.g., using `ptrace` or another method)

---

<!-- slide template="[[Base Slide]]" -->
::: title
#### Implant → Execution: Payload Format
:::

- Your payload should be available as:
  - An **ELF binary**.
  - A **Shared Object (SO)**.
- A  **reflective Shared Object** (rSO)  
- BONUS: **Position‑Independent Code (PIC)** (shellcode that can be injected).

---

<!-- slide template="[[Base Slide]]" -->
::: title
#### Implant → File I/O
:::

- The implant must:
  - Read files from the target machine and send them back to the C2.
  - Download files from the C2 and write them to disk (encrypted or plaintext).
- Although the task seems simple, secure file handling on Linux can be challenging—pay close attention to input sanitization and error handling.
- further, make sure to handle large files in memory constrained enviornments

---

<!-- slide template="[[Base Slide]]" -->
::: title
#### Safe File Upload
:::

- **Spot the error:**
- write an exploit
  ```python
  @app.request("/upload_json", methods=["POST"])
  def handle_upload():
      json_data = request.json
      data = json_data.get("data")
      raw_bytes = decode_data(data)
      filename = json_data.get("filename")
      loot_path = "static/" + filename
      with open(loot_path, 'wb+') as f:
          f.write(raw_bytes)
          print("I am 1337 and wrote my loot to", loot_path)
      return jsonify({"status": "ok"})
  ```

---

<!-- slide template="[[Base Slide]]" -->
::: title
#### Safe File Upload (Cont'd)
:::

- **Question:** Do we actually control the “filename” parameter?
- Always sanitize and validate all untrusted inputs.

---

<!-- slide template="[[Base Slide]]" -->
::: title
#### Safety
:::

- If your server can be compromised with minimal effort, expect a significant penalty.
- Rigorously sanitize all untrusted inputs.
- this also applies to parsing file formats

---

<!-- slide template="[[Base Slide]]" -->
::: title
#### Implant → Persistence
:::

- Your implant must persist across reboots on the target Linux system.
- **Techniques may include:**
  - Installing a systemd service.
  - Adding a cron job.
  - Modifying shell startup files (e.g., `.bashrc` or `.profile`).
- **For full credit:** Implement at least one persistence strategy.
- **Bonus:** Tailor persistence so that the implant runs only on a specific hostname or when a particular MAC address is detected.

---

<!-- slide template="[[Base Slide]]" -->
::: title
#### Implant → Loot
:::

- Programmatically extract sensitive data from the target machine. 
	- i.e. glob, ssh keys, ...etc
	- anything else that might help you enumerate the machine
---

<!-- slide template="[[Base Slide]]" -->
::: title
#### Implant → Defense Evasion
:::

- **Required:**  
  - Encrypt or obfuscate configuration strings so that defenders cannot simply run `strings` on your binary.
  - Avoid overusing system shell commands (e.g., excessive calls to `/bin/sh`) when gathering situational data.
- **Options include:**
  - A crypter that “packs” and obfuscates your payload.
  - Techniques for evading Linux security mechanisms (e.g., AppArmor or SELinux evasion).
  - API hashing or dynamic symbol resolution.
  - Mimicking a legitimate service in your RPC communications.
  - Tailoring the payload to the target environment.
  - Any creative alternative approved by course staff.

---

<!-- slide template="[[Base Slide]]" -->
::: title
#### Operational Security (Opsec)
:::

- **Capstone guidelines:**
  - Minimize writing files to disk.
  - Limit the use of shell calls or external scripts.
  - Restrict memory regions with R/W/X permissions.
  - Avoid spawning excessive processes or threads.
  - Separate low‑latency communications (e.g., check‑ins) from high‑latency tasks (e.g., data exfiltration).
  - Keep your payload modular and small; load additional functionality on demand.
  - Be prepared to explain *why* your implant behaves as designed.
  - Reduce human‑readable strings and overall file size.

---

<!-- slide template="[[Base Slide]]" -->
::: title
#### General Comments
:::

- Course staff will support projects that:
  - Implement the C2 server and client in Python.
  - Use a message broker (e.g., RabbitMQ, Redis/valkey, or ZeroMQ).
  - Develop the implant in C/C++ or another systems language (C, C++, Zig, Rust, or Assembly for extra credit).
- **Important:**  
  - Do **not** use high‑level languages with garbage collectors (e.g., Python, Go) for the implant.
  - Your code **must compile and run on an aarch64 Linux system**.
  - make my life easy.  "conf.h", "make all"
  

---

<!-- slide template="[[Base Slide]]" -->
::: title
#### General Requirements: C2
:::

- Your Command and Control Server must handle connections from multiple operators and implants.
- While you may choose any language, recommended components include:
  - **Backend:** Flask (paired with a WSGI server like Gunicorn) for HTTP handling, twisted for tcp.
  - **Database:** PostgreSQL or MySQL for structured data storage.
  - **Message Broker:** RabbitMQ, ZeroMQ, or Redis for mediating communications between implants and clients.

---

<!-- slide template="[[Base Slide]]" -->
::: title
#### C2 → Database
:::

- Your database should include tables for:
  - Implants
  - Commands
  - Jobs (in‑progress/finished tasks)
  - Clients (operator sessions)
- Ensure your schema supports tracking and auditing of key events.

---

<!-- slide template="[[Base Slide]]" -->
::: title
#### C2 → Database → Implants
:::

- **Data fields to consider:**
  - **Implant ID:** A unique identifier.
  - **Hostname:** The target machine’s name.
  - **Username:** The user under which the implant is running.
  - **Machine ID:** (e.g., contents of `/etc/machine-id`).
  - **Privilege Level:** Root or non‑root, group id, ...etc.
  - **Connecting IP Address:** Source IP of the implant.
  - **Session Key:** The key negotiated during the handshake.
  - **Sleep Interval & Jitter:** Timing parameters.
  - **First Seen / Last Seen / Expected Check‑in:** Timestamps for tracking.

---

<!-- slide template="[[Base Slide]]" -->
::: title
#### Messaging 
:::

- Operators should be notified when:
  - A new implant connects for the first time.
  - A new client (operator) connects.
  - A command is issued.
  - An implant responds to a task.
- **Tip:** RabbitMQ Pub/Sub is an effective method for handling notifications.
- Your client can run on the same machine as the C2 (assuming SSH access) or connect via HTTP endpoints.

---

<!-- slide template="[[Base Slide]]" -->
::: title
#### Client
:::

- The client must securely connect to the C2, send and receive data, and display updates from implants.
- It can be terminal‑based (using libraries like prompt‑toolkit) or a web interface with secure authentication.
- easiest is probably to use wireguard or a ssh tunnel
---

<!-- slide template="[[Base Slide]]" -->
::: title
#### Special Feature	
:::

- You must implement one advanced feature in your C2.
- This may be an advanced implant command, an enhanced RPC mechanism, a polished UI element, or another innovative functionality.
- Propose your idea and obtain final approval from course staff.
- In rare situations,  the special feature can grantee you an automatic A in the class. 

---

<!-- slide template="[[Base Slide]]" -->
::: title
#### Special Feature: Implant
:::

- Examples include:
  - API hashing for dynamic resolution.
  - Tailored payloads.
  - An initial access payload (e.g., a malicious shell script or compromised package installer (deb)).
  - Techniques for evading security (such as randomized imports).
  - Proxy pivots (port forwarding).
  - A P2P‑based C2 (which might earn extra credit).
  - Custom loaders or direct system call integration.

---

<!-- slide template="[[Base Slide]]" -->
::: title
#### Special Feature: RPC
:::

- Options include:
  - Computation‑based sleeping.
  - Advanced cryptography (e.g., Password Authenticated Key Exchange).
  - Full control of your C2 via an API.
  - Using Protocol Buffers for RPC/gRPC.
  - Steganography.
  - Alternate C2 channels such as DNS, WebSockets, or SMTP.
  - Malleable, customizable RPC protocols.

---

<!-- slide template="[[Base Slide]]" -->
::: title
#### Special Feature: UI
:::

- Use libraries like Rich or prompt‑toolkit to build an attractive, user‑friendly interface.
- Consider implementing an HTTP panel with secure authentication for controlling implants.
- Creative design ideas are welcome!

---

<!-- slide template="[[Base Slide]]" -->
::: title
#### Special Feature: Other
:::

- Clever “trolling” is encouraged if it’s inventive—your implant might, for instance, perform humorous actions (beyond playing music or moving the mouse).
- You might also containerize your entire project.
- Have a creative idea? Propose it and get approval from course staff.

---

<!-- slide template="[[Split Vertical]]" -->
::: title
#### Example: Troll 
:::

::: left
- Imagine steganography that hides a final payload in an innocuous image.
:::
::: right
![[0x0c_slide_49_image.jpg|300]]
:::

---

<!-- slide template="[[Base Slide]]" -->
::: title
#### Remarks
:::

- **All code must run on an aarch64 Linux system** (physical hardware or via an emulator/VM).
- Document every command and function.
- Ensure your code is well-organized, modular, and commented.
- **Guidelines:**
  - Functions should generally be fewer than 50 lines.
  - Source files should not exceed 500 lines.
  - Provide clear examples for using the client, server, and implant.
- Your project must build on a Linux environment—avoid Windows‑only toolchains.

---

<!-- slide template="[[Base Slide]]" -->
::: title
#### Build Recommendations 
:::

- Use GCC or Clang on aarch64 Linux.
- GNU Make is sufficient; CMake is acceptable if documented well.
- For resource‑based configuration, consider compiling in configuration files or resources.
- Use Linux package managers (e.g., apt, pacman) to manage dependencies.

---

<!-- slide template="[[Base Slide]]" -->
::: title
#### Build Requirements
:::

- Implement a builder for your implant.
- The builder should allow recompiling the implant with custom configuration (for example, to change the C2 server address).

---

<!-- slide template="[[Base Slide]]" -->
::: title
#### The Best C2 Will Get a Prize :-)
:::

- The team with the “best” C2, as judged by course staff, will receive a mystery reward.
- Judging criteria include:
  - **Opsec:** Is your C2 stealthy and low‑noise? Is it difficult for Linux defenders (or tools like SELinux/AppArmor) to detect?
  - **Documentation:** Is it clear and easy to follow?
  - **Special Feature:** How innovative or “cool” is your added functionality?
  - **UI/UX:** Is your interface user‑friendly?
  - **Humor:** Did it make me laugh?

---

<!-- slide template="[[Base Slide]]" -->
::: title
#### Projects You Can (and Should) Use for Reference 
:::

- [Metasploit Payloads](https://github.com/rapid7/metasploit-payloads) – primarily C‑based; review for general techniques.
- [mettle](https://github.com/rapid7/mettle) minimal meterpreter implant
- [Mythic C2](https://docs.mythic-c2.net/) – an advanced, open‑source C2 framework that supports multiple platforms.
-  [VX Underground mal src](https://github.com/vxunderground/MalwareSourceCode/tree/main/Linux)
- [Posh c2 Implant linux](https://github.com/nettitude/PoshC2_Linux_Implant)
- [shellnova](https://github.com/zer1t0/shellnova) template for writing x86 shellcode from c code. good for reference 
- [dnscat](https://github.com/iagox86/dnscat2) dns remote access trojan 
-  [hacking team linux leak](https://github.com/hackedteam/core-linux/tree/master) Italian spyware company implant leak


---

<!-- slide template="[[Base Slide]]" -->
::: title
#### Poster Requirements 
:::

- **Include:**
  - Names (or handles) of each group member.
  - The name of your malware.
  - A summary of its basic features.
  - An architecture diagram illustrating how your malware operates and is controlled.
  - An operational security analysis.
  - A discussion on potential use cases.
  - Associated IOCs (Indicators of Compromise).

---

<!-- slide template="[[Base Slide]]" -->
::: title
#### Demo Requirements 
:::

- **Deploy your malware** on an aarch64 Linux VM, physical machine, or cloud service.
- Demonstrate:
  - The implant “detonating” (running).
  - The C2 server notifying operators of a new implant connection.
  - Examples of command execution (e.g., running shellcode or commands).
  - Looting functionality.
  - Your special feature.
- Discuss  tactics/techniques and how defenders might detect your implant.
- Live demo of sifting through the target environment 

---

<!-- slide template="[[Base Slide]]" -->
::: title
#### Writeup
:::

- Your writeup can be a formal report or a README.md in your repository.
- **Must include:**
  - Detailed documentation for your C2 (commands, architecture, usage examples).
  - A discussion of the techniques used.
  - Operational security considerations.
  - Detection rules and countermeasure analysis.
  - A rationale for your design choices and a description of your C2’s architecture.

---

<!-- slide template="[[Base Slide]]" -->
::: title
#### Timing 
:::

- Late submissions for the writeup may be accepted.
- **No late submissions** for the poster session or live demo.
- Attendance at the poster session is mandatory.
- Industry professionals (approximately five) will be present—an excellent networking opportunity!

---

<!-- slide template="[[Base Slide]]" -->
::: title
#### C2 Engineering Basics
:::

- Send data.
- Receive data.
- **Profit.**

---

<!-- slide template="[[Base Slide]]" -->
::: title
#### Team Server
:::

  - Multiple implants will connect to your C2.
  - Multiple operators will use your C2 to control implants.
  - Operators need to receive real‑time updates.
- Examples of notifications:
  - A new implant connects.
  - An implant downloads a task.
  - An operator issues a command.
- Your team server should distribute messages efficiently.

---

<!-- slide template="[[Base Slide]]" -->
::: title
#### Server Components and Concepts 
:::

- **Listeners:** HTTP or other protocol listeners for implant connections.
- **Database:** A system to store information about implants, operators, and messages.
- **Messaging:** Mechanisms for sending real‑time updates to operators.

---

<!-- slide template="[[Base Slide]]" -->
::: title
#### Databases 
:::

- Databases store structured information.
- For our purposes, a SQL‑based database (such as PostgreSQL or MySQL) is ideal



---

<!-- slide template="[[Base Slide]]" -->
::: title
#### SQL Databases: The Big 4
:::

- **SQLite:** Lightweight and file‑based; good for POCs but may not scale.
- **MySQL:** Scalable for single‑machine deployments; simple to manage.
- **PostgreSQL:** Feature‑rich and scalable—recommended for complex schemas.
- **Microsoft SQL:** Typically not used in Linux environments.



---

<!-- slide template="[[Base Slide]]" -->
::: title
#### Really, Again with Learning Another Language?
:::

- Many organizations store critical data (e.g., customer or payment information) in databases, so understanding SQL is crucial.
- Plus you can use ORMs which greatly simplify the development on your end

---

<!-- slide template="[[Base Slide]]" -->
::: title
#### SQL: Seriously Oversimplified 
:::

- Think of a SQL database as a set of spreadsheets:
  - **Tables:** Like individual sheets.
  - **Rows:** Each row is a record.
  - **Columns:** Each column has a name and data type.
- SQL (Structured Query Language) lets you query and manipulate this structured data easily.

---

<!-- slide template="[[Base Slide]]" -->
::: title
#### Which Database Should You Use for Your C2?
:::

- **Key questions:**
  - How many implants do you expect?
  - How many operators will be active concurrently?
  - How much structured data must you store?
  - Which C2 channel and RPC mechanism are you using?

---

<!-- slide template="[[Base Slide]]" -->
::: title
#### Don't Use SQLite Past the POC Phase
:::

- SQLite operations are blocking and support only one connection at a time.
- This can lead to slow performance when multiple connections are made concurrently.

---

<!-- slide template="[[Base Slide]]" -->
::: title
#### Recommendation: Stick to PostgreSQL or MySQL
:::

- Both are production‑ready, powerful, and can handle complex schemas and high connection counts.
- PostgreSQL is especially robust when scaling up.

---

<!-- slide template="[[Base Slide]]" -->
::: title
#### SQLAlchemy
:::

- Use SQLAlchemy (or Flask‑SQLAlchemy) to interact with your database in Python without writing raw SQL.
- This simplifies CRUD operations.
- [Quickstart Guide](https://flask-sqlalchemy.palletsprojects.com/en/2.x/quickstart)

---

<!-- slide template="[[Base Slide]]" -->
::: title
#### Example Using SQLite3
:::

- **Workflow:**
  - Establish a connection to the database.
  - Execute SQL commands.
  - Use a “cursor” object to iterate over results.
- Note: While SQLite is fine for testing, use PostgreSQL or MySQL for production.

---

<!-- slide template="[[Split Vertical]]" -->
::: title
#### Remarks about Safety
:::

::: left
- Using an ORM like SQLAlchemy is generally safer than writing raw SQL queries.
- Always sanitize inputs and use parameterized queries.
:::
::: right
![[0x0c_slide_72_image.png|300]]
:::

---

<!-- slide template="[[Title Slide]]" -->
## Discussion: Drawing Out a C2 Framework 

---

<!-- slide template="[[Base Slide]]" -->
::: title
#### Flask 
:::

- Flask is a lightweight HTTP server framework.
- It uses decorators for routing.
- Its ecosystem includes many useful plugins for database integration and more.
- **Note:** Flask isn’t production‑ready by itself—pair it with a WSGI server like Gunicorn.



---
<!-- slide template="[[Base Slide]]" -->

::: title

#### twisted

:::
- python3 library for making network program not suck
- makes writing tcp based protocols straight forward
- example: implementing something like metasploit multi handler 

---

<!-- slide template="[[Base Slide]]" -->
::: title
#### Final note: Messaging 
:::

- you need Standardize messages between operators and the server.
- you also need to have a fixed message format  for implant to server 
- JSON is a popular format due to its ease of parsing and serialization.
- Group messages by event type (e.g., `New_implant_event`, `Implant_checkin_event`, `Implant_response_event`).
- Operators can subscribe to specific event topics to stay informed.


---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Coming Soon:

:::
- stay tuned for SPEDRUN-2025. 
- wear your best hacker attire 
- glowsticks encouraged 
- taking requests for D&B sets (min 160 BPM)
