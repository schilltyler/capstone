
---
<!-- slide template="[[Base Slide]]" -->
::: title
#### More Definitions
:::
- **Indicators of Compromise (IOCs)**: sets of forensic data found when malicious activity occurs (IPs/domains of a C2 server, hash values of malware, suspicious email accounts, etc.).
- **Hash value**: a fingerprint for data.
- **IP address**: the address of a computer on a network.
- **Domain Name**: a mapping (A/AAAA record) from a human-friendly name (e.g., `example.com`) to an IP address (e.g., `192.0.2.44`).




---
<!-- slide template="[[Base Slide]]" -->
::: title
#### Common IOCs
:::

- Domains / URLs / IPs 
- Named pipes/sockets, paths
- Debug info, special strings
- Files/folders touched
- Executable hash (md5/sha256/blake2/“import” style)
- YARA Rule match
- Embedded configuration or data sections


---
<!-- slide template="[[Split Vertical]]" -->
::: title
#### Threat Analysis Pyramid of Pain
:::

::: left
<!-- element style="font-size: 24px"-->
- IOCs help track and identify threats
- IOCs should be simple values that can be easily shared
- Considering **context** across multiple IOCs helps identify TTPs
:::

::: right
![[0x03_slide_8_image.png|500]]
:::

---

<!-- slide template="[[Base Slide]]" -->
::: title
#### From IOCs to TTPs
:::

#### Hash -> Tools
- What malware family does this hash belong to? 
  - What does it do?
- Are multiple hashes related? 
  - Are they versions of the same malware or different tools?

---

<!-- slide template="[[Base Slide]]" -->
::: title
#### From IOCs to TTPs
:::

#### IP/Domain -> Network Artifact
- How does the malware communicate with C2?
- Look for patterns in network traffic. Can we block that behavior?
- Are the domains known bad, or newly registered? 
- Is the IP compromised infrastructure? 
- Which ASN does it belong to?

---

<!-- slide template="[[Base Slide]]" -->
::: title
#### From IOCs to TTPs
:::

#### IOC -> TTPs 
- Is there a pattern in how this group operates?
- Do they drop multiple malware families?
- How do they deliver the malware? 
- Who do they target?
- **How do we block this activity?**

This is often *very* difficult.

---

<!-- slide template="[[Base Slide]]" -->
::: title
#### Easy…until it isn’t!
:::

- Depending on the actor, the Pyramid of Pain is just pain
- Hashes are easy — unless there’s polymorphic code
- IPs are easy — unless the actor uses thousands of compromised IP addresses
- Domains are easy — unless there's a domain generating algorithm (DGA) or mass domain hijacking
- Or if they use popular apps/platforms like Slack or GitHub as C2
- **It quickly becomes complicated**

---

<!-- slide template="[[Split Vertical]]" -->
::: title
#### Different Companies See Different IOCs
:::

::: left
- Anti-virus/EDR vendors see actual malware
- Email providers see phishing emails
- Domain registrars see domain ownership
- Nobody sees everything. 
- Collaboration is crucial: **analysis is hard**
:::

::: right
![[0x03_slide_12_image.jpg|500]]
:::
