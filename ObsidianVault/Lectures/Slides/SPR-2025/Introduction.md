---
slideNumber: true
theme: night
---

---
<!-- slide bg="[[Green Logo.svg]]" background-size="auto" -->


---
<!-- slide template="[[Split Vertical]]" -->
::: title
#### CS-4973/6983
:::
::: left
![[Green Logo.svg | 400]] <!-- element style="object-fit: cover" -->

:::
::: right
<!-- element style="font-size: 32px;align:center" -->
Introduction to Malware, Threat Hunting & Offensive Capabilities Development
:::



---
<!-- slide template="[[Title Slide]]"-->
#### Ch0nky LTD
Why teaching kids to write implants is actually a good idea


---
<!-- slide template="[[Split Vertical]]" -->
::: title
 #### $ whoami  
:::

::: left
<!-- element style="font-size: 24px"-->
- Kai 
- Security Researcher
- Lecturer + PHD candidate @ NEU
	- I have two emails and I check neither of them
	- Please ping me on discord if you send me an email.
 
:::

::: right
![[lemon-hammock.jpg|270]]


:::

:::

---
<!-- slide template="[[Title Slide]]" -->
 ## Course Overview
---
<!-- slide template="[[Base Slide]]" -->
::: title
#### Abstract 

::: 
![[Abstract]]




---
<!-- slide template="[[Base Slide]]" -->
::: title
#### Benefits of Learning to Write Malware  
:::

- The first thing I do when learning about a new system/architecture is write a poc implant targeting it.
- usually this means writing a simple loader/beacon
- Loading code, and communicating to a remote C2
	- C2 Channels 
- trojan horsed way of teaching Systems security :)

---
<!-- slide template="[[Base Slide]]" -->
::: title
#### Getting Started: 
:::
- Esoteric knowledge of the target 
- Computer Systems/Operating Systems
- Computer networking 
- cryptography 
- linking/loading 
---
<!-- slide template="[[Split Vertical]]" -->
::: title
 #### Why get into Infosec
:::

::: left
- It’s fun :D.
- There is no shortage of interesting work. 
- The impact you can have is massive
- **very** generous compensation.
- Deeper understanding of the types of technology that power the modern world
- You’ll become really fun at parties. 

:::

::: right
![[why-infosec.png|350]]


:::

---
<!-- slide template="[[Base Slide]]" -->
::: title
#### Goals of the Course	 

:::

- Learn basic Linux internals and `Armv8-a`  development
- Create a safe environment  to allow students to explore malware
- Create clear paths for students to enter the security community 
- Help make everyone here more security literate.	
- Get your hands dirty reversing and writing malware
- You read that right. In this class you will be writing malware<!-- element class="fragment highlight-red" -->


---
<!-- slide template="[[Base Slide]]" -->
::: title
#### [[MalwareCourse/Capstone]] 

:::

- Over the course of a semester, students will reverse engineer and re-implement capabilities of malware.
- The course capstone is to  implement a production ready Remote Administration Tool (RAT) .
- The implant will be detonated on a raspberry pi 5 (8gb) running Pi Os (Debian derivative)
- You have to work in a groups for the final capstone project.
<b><center>** Infosec is a Team sport!**</center> </b> 
- Go make friends with your neighbor 

---
<!-- slide template="[[Split Vertical]]" -->
::: title
#### Capstone Progression
:::

::: left
- You will learn how malware works by reverse engineering samples, then re-implementing the capabilities observed.
- With each new iteration, you will build out more components of your implant
:::
::: right
![[Ch0nkyBear.png|350]]
:::

---
<!-- slide template="[[Title Slide]]" -->
 ### This is a new field. All concepts learned in this class are directly applicable to industry. 

---
<!-- slide template="[[Base Slide]]" -->
::: title
#### Course Success Stories	

:::

<center>For students who demonstrate passion for the subject, I will personally leverage my network to help you find a job</center>

Other incentives include:
- Really cool Ghost stickers
- Free trip to DefCon
- Introduction to folks from industry
- Jobs lined up for high performers in the course 

---
<!-- slide template="[[Split Vertical]]" -->
::: title
 #### [[Syllabus#Tentative Topic list]]
:::

::: left
- TLDR: C, Armv8-A, Linux (mostly userland) Internals, Ghidra, implant dev/rev
- This is the sixth time the course is being taught. 
- This is the **first**  time the it focuses on Linux
	- There are going to be hiccups, and I am writing a large part of the course material on the fly!
- Depending on how things go, we may cover more or less than what is listed on the syllabus.
- Since this is designed to be current, topics might shift according to current events. 

:::

::: right
![[slide_8_image.png|260]]


:::


---
<!-- slide template="[[Base Slide]]" -->
::: title
#### Why should you not take this class this year
:::
- its new :)
![[ben-break.jpeg]]

---
<!-- slide template="[[Base Slide]]" -->
::: title
#### Why should you not take this class this year
:::
- This class is not easy. Far from it.  
- This class is taught in c. 
- c will happily let you blow your foot off 
- There are plenty of opportunists for your to accidentally spend 30 hours on a problem only to find it was as simple as `i==0` instead of `i=0`
- You will almost certainly spend hours debugging code
- The capstone project is  very hard.
- **I say this not to scare you, but to be clear: you need a certain amount of academic maturity to succeed in this course! More on difficulty later...** 

---
<!-- slide template="[[Title Slide]]" -->
 ###  Baseline Course Policies
---
<!-- slide template="[[Title Slide]]" -->
 ### Read the [[Syllabus]]
 If you ask me a question that is answered on the syllabus, I will gently direct you there...
 
---
<!-- slide template="[[Base Slide]]" -->
::: title
#### [[Policies#Attendance]]
:::

- Attendance  is mandatory. Unless cleared with me, you may not asynchronously take the course. 
- While I record lectures, they are often released 2-4 days after they are recorded. 
- I do not take attendance, but I do give regular quizzes at the start of class.
- If you need to miss class for any reason, please contact me
- If I have any reason to believe you are not physically in Boston for the semester, that Is a problem
- Lecture: 1 hour, then 1 hour for dinner/lab then 1 hour
- There is a 0% chance I will lecture non stop for 3.5 hours


---
<!-- slide template="[[Base Slide]]" -->
::: title
#### [[Policies#Communication]]

:::

- We have a dedicated [[Discord]] server  and you are expected to join. Please see the course docs for getting started.
- I will also respond to email, but will be slower there (at least 24-48 hours)
- If you have a personal question about course administration, job stuff, or anything else that you don’t think your fellow classmates can benefit from, you may DM
- If someone else can benefit from the question, please ask it publicly. 
- If you need help, Ask

---
<!-- slide template="[[Base Slide]]" -->
::: title
#### Academic Honesty

:::

- TLDR: Don’t cheat. Thnx
- Cheating and plagiarism of any kind will not be tolerated. 
- Cheating robs you of the experience of writing your own code, and in turn is a waste of your time and money.
- I will not tolerate anything that compromises your ability to learn in this class.
- You’re all adults, and know the drill. 


---
<!-- slide template="[[Base Slide]]" -->
::: title
####  Use of ChatGPT/LLMs
:::

-  :smile:  You are welcome to use LLMs to help explain documentation.
- :smile: You can ask LLMs to help you find bugs in your code or to diagnose compiler errors. 
- :smile: You can (try) asking for inspiration for your final project.
- :smile:  You may use LLMs to help write documentation, unit tests and comments
- :no_entry_sign: You may **not** copy and paste generated code and submit it as your own.
-  If I catch you submitting generated code, you are  going to have a bad time.
- I can't stop you from pasting assembly/decompiled code and pasting it into Chat GPT (i do this during CTFs all the time!) but am going to advise against it. If you don't learn the basics, you are going to **suffer greatly** on something LLMs can't help with 

---
<!-- slide template="[[Base Slide]]" -->
::: title
#### Academic Honesty (cont)

:::

- If you submit code that is identical to another classmates, you will fail.
- If you collaborate when it is expressly forbidden, you will fail.
- If you copy and paste code you don't understand in the real world, you are incurring a lot of personal and institutional risk.
- If you would like to work in a group for homework assignments you can where permitted, but you can’t both submit the exact same code. You also need to clearly identify who you worked with and where your sources came from. 

---
<!-- slide template="[[Split Vertical]]" -->
::: title
 #### Academic Honesty (cont)
:::

::: left
- I am not a cop. 
- You are paying a lot of money to be here. 
- It is my job to make sure you get your monies worth
- It is your job to give this class your all

:::

::: right
![[neu-is-expensive.png| 300]]

:::

---
<!-- slide template="[[Split Vertical]]" -->
::: title
 #### Grading Policies
:::

::: left


-  ** This is not a weed out course. **
- You get what you put in, use the time to learn and try new things.  
- There is a curve, and the average is a B+ (A- for grad)
- If you are on the border of a letter grade, I will probably bump it.
- I do not curve grades down
- If everyone in the class crushes it, then everyone gets an A.
- In general, it is common to get the average, rare to fail, and difficult to get an A. 
:::

::: right
![[slide_22_image.png|300]]


:::

---
<!-- slide template="[[Split Vertical]]" -->
::: title
#### Difficulty	

:::
::: left 

<!-- element style="font-size: 24px"-->
- This class is not easy
- It is easy to spend hours on problems that are simple in hindsight
- The capstone will be very time consuming and will require you to work in a group
- You may not work solo unless you're a Grad student.  
:::
::: right
<!-- element style="font-size: 24px"-->
> [!tip]Trying hard literally pays off!
 Reminder: many students have received job offers based on the work they did for the capstone!
:::
---
<!-- slide template="[[Split Vertical]]" -->
::: title
 #### Difficulty (cont)	
:::

::: left
- Coding assignments will require you to read  documentation
- Reading the documentation can feel tedious but is a valuable skill in and of itself.
- Being a hacker means caring about material that others overlooked/trusted implicitly
- As a hacker, you should enjoy getting into the weeds **
(Or at the very least not be afraid to dive in :) 
:::

::: right
![[slide_24_image.png|500]]


:::

---
<!-- slide template="[[Base Slide]]" -->
::: title 
#### Difficulty (cont)
:::
- Part of reverse engineering is analyzing techniques you have not seen before
- Assignments will regularly have content that has not been previously covered 
- There are no stupid questions but I might tell you to Google it :-)
- Know that when I do send you to lmgtfy, I am not being catty. Finding answers to your own questions is an essential skill!. If it is a question that I think you can answer yourself, I will polietly encourage you to do so. 
---
<!-- slide template="[[Split Vertical]]" -->
::: title
 #### Difficulty (cont)	
:::

::: left
`<rant>` 
- Most of this content is simple in hindsight
- Simple != easy
- Getting comfortable being lost and 
working  out what is happening  is an essential skill for hacking  

`</rant>`
See [[On Simplicity]]
:::

::: right
![[slide_25_image.png|450]]


:::

---
<!-- slide template="[[Title Slide]]" -->
 #### [[Policies#No Jail]]
---
<!-- slide template="[[Base Slide]]" -->
::: title
#### [[Policies#No Jail]]
:::

- I AM NOT A LAWYER. (IANAL)
- The  Computer Fraud and Abuse Act 1986: bans “
intentionally accessing a computer without authorization or in excess of authorization”.
- We will not protect you if you conduct operations or deploy any malware / tools outside your approved lab environment.
- We will not bail you out of jail

---
<!-- slide template="[[Base Slide]]" -->
::: title
#### [[Policies#No Jail]]
:::

 - Do not piss off anybody who has more time and money than you. The laws are intentionally vague, and the 
	- Judges are technically illiterate
	- You will lose and be made an example of. 
- When in doubt, ask course staff but always error on the side of caution!    


---
<!-- slide template="[[Split Vertical]]" -->
::: title
 #### Seriously, Don’t Hack Without Permission	
:::

::: left
<!-- element style="font-size: 24px"-->
- Aside from it being illegal,  immoral, creepy, and risky, you will fail the class.
- Plus,  the payout most criminal hackers get is considerably less than if you do the same job but call it “adversary emulation” or run a threat intel company. 
- Don't ruin it for everyone else. 

:::

::: right
![[slide_28_image.png|400]]


:::

---
<!-- slide template="[[Base Slide]]" -->
::: title
#### Do not F\*ck with the Government

:::

- The current geopolitical climate is hot.
- Countries haven’t regularly resorted to kinetic action in response to hacking, but that isn’t a hard and fast rule. It is a norm that might not be respected tomorrow 
- Do not, under any circumstances, hack a foreign or domestic government. They have all the time, all the money, and capabilities that you cannot hope to compete with. **You will lose** and **you will be caught** 
- If you really want to do this kind of work, go get a clearance.
- ** *Kinetic***   is a euphemism for killing people. 

---
<!-- slide template="[[Base Slide]]" -->
::: title
#### Use caution when publishing tools 

:::

- Plenty of researchers publish offensive security tools
- Some advance the field, some are published for clout. Be careful 
with
 especially sophisticated, easy to use, and low detection tools.
- Our job is to advance the security industry, not make script kiddies look good.
- Think long and hard about the implications of 
submitting
 (for example) a pull request to metasploit or 
open
 sourcing your final project
---
<!-- slide template="[[Base Slide]]" -->
::: title
#### Be Thoughtful 

:::

- Threat actors use open source tools. 
- Don’t be the reason that a hospital gets hit with ransomware.
- If you do decide to publish tools, make sure to also include countermeasures that defenders can use to combat your tool.
- For questions about licenses, responsible disclosure, or advice on whether or not to publish a tool, please contact us! We are happy to provide feedback.

---
<!-- slide template="[[Title Slide]]" -->
 ## [[Policies#Grading]] 
---
<!-- slide template="[[Base Slide]]" -->
::: title
#### [[Policies#Late policy]] 

:::

- You are allocated 3 late days for the semester, and may use them at your discretion. 
- You may ask for more late days for extenuating circumstances. I am pretty reasonable when it comes to providing extensions.
- Late days may only be used on reverse engineering assignments, or coding assignments.
- There are no late days for the capstone project or exercises.
- I do not accept submissions for exercises if you are remote.


---
<!-- slide template="[[Split Vertical]]" -->
::: title
 #### Assignment Philosophy: 
:::

::: left

- The questions on homework assignments will increase in difficulty.
- Difficulty should be a spectrum
- Often times the solution is simple
- Again: Simple != easy
- You are rarely expected to complete the totality of an assignment 

:::

::: right
![[slide_34_image.png|300]]


:::

---
<!-- slide template="[[Split Vertical]]" -->
::: title
 #### RE: Reverse Engineering HW
:::

::: left
- Automatically graded for the most part in the form of CTFs
- Will be time consuming.
- You are encouraged to to collaborate with classmates only where explicitly permitted 
- **the default policy is no working in groups**
:::

::: right
![[slide_35_image.png|500]]


:::

---
<!-- slide template="[[Split Vertical]]" -->
::: title
 #### Re: Offensive Labs
 :::

::: left
<!-- element style="font-size: 24px"-->
- Assignments are automatically graded
- Partial credit is given on a case by case basis
- Labs consist of capabilities to add to a "shell"
- Object files are provided in lieu of solutions to ensure students don't fall behind
- Labs build up towards the capstone 
:::

::: right
![[htop_spongebob.png]]
:::

---
<!-- slide template="[[Split Vertical]]" -->
::: title
 #### RE: Coding Assignments
:::

::: left
- Coding assignments are auto graded.
- Unless otherwise stated, you may NOT work in groups
assignments.
-  You may only collaborate when it is explicitly stated. 

> [!tip] Coding assignments are meant to be completed solo!
:::

::: right

![[no_working_in_groups.png]]

:::

---
<!-- slide template="[[Split Vertical]]" -->
::: title
 #### RE: Exercises 
:::

::: left
<!-- element style="font-size: 24px"-->
- Exercises are weekly, quick turn assignments (usually in person)
- They are designed to be sanity checks for topics as the course progresses
- Should take 5-10 minutes 
- Questions will always be based on recent material, but topics build on each other
- Bring your laptop to class.

:::

::: right
![[slide_38_image.png|300]]


:::

---
<!-- slide template="[[Split Vertical]]" -->
::: title
 #### CTF
:::

::: left
- There are a few ways to fail the class. One of them is failing to sign up for the ctf. the CTF is primarily how grading will be handled. 
- There will be a prize :cowboy_hat_face:

:::

::: right
![[slide_39_image.png|300]]


:::

---
<!-- slide template="[[Base Slide]]" -->
::: title
#### Grading Breakdown

:::

#### Subject to change. 
Single source of truth is in the Syllabus 
- Malware Write up: 10%  
- Exercises: 5%
- Capture the Flag (CTF): 10%
- Reverse Engineering Assignments: 15% 
- Coding Assignments: 10%
- Capstone Project: 50% 

#### Bonus Points:
- Class Participation: 2.5% 
- Guest lecture attendance  2.5%
- Vibes (20%+)
	- ???


---
<!-- slide template="[[Split Vertical]]" -->
::: title
 #### Remarks about Mental Health
:::

::: left
- I give incompletes to students who have extenuating circumstances: usually medical emergencies.
- Mental health emergencies are medical emergencies.
- There will be some sections that discuss material that may be triggering to some. 
- Warnings will be given over Discord and at the start of such lectures, and attendance is completely optional. 
:::

::: right
![[slide_41_image.png|400]]


:::

---
<!-- slide template="[[Base Slide]]" -->
::: title
#### To Reiterate

:::

- I give incompletes.
- If you say “I am having a medical emergency and need an incomplete” you will get one;  zero questions asked. 
- Note this means you get about 30-90 extra days to complete the outstanding work. 
---
<!-- slide template="[[Title Slide]]" -->
 ## Course Lab Environment 
---
<!-- slide template="[[Split Vertical]]" -->
::: title
 #### Malware = Bad
:::

::: left
<!-- element style="font-size: 24px"-->
- In this class you will be analyzing and running real malware
- In order to reduce the risk of infecting your machine, all suspicious binaries should be run in a malware sandbox
	- This is a way for us to isolate potentially malicious code from  system resources.


:::

::: right
![[slide_44_image.png|300]]


:::

---
<!-- slide template="[[Split Vertical]]" -->
::: title
 #### [[Virtualization]]
:::

::: left
- Hypervisor: Software to run virtual machines.
- KVM: Kernel Virtual Machine:  hypervisor used to run virtual machines 
- LibVirt: library for interacting with hypervisors 
- `virsh`: shell built using libvirt to automate  VM management 
- for more see **[[Virtualization]]**



:::

::: right
![[slide_45_image.png|700]]


:::

---
<!-- slide template="[[Split Vertical]]" -->
::: title
 #### [[Sandbox]]
:::

::: left
- Your first assignment is to create a malware analysis sandbox. 
- The analysis sandbox will be composed of  a Hypervisor running two virtual machines 
- Sandbox: guest machine for detonating malware.  
- Gateway:  guest machine with (optional) access to internet.  
- If you choose to use an emulator, the gateway will be the sandbox 

:::

::: right
![[MalwareLab.excalidraw.svg]]

:::

---
<!-- slide template="[[Base Slide]]" -->
::: title
#### Course Lab Environment 

:::

- This course is designed and tested for virtual machines running on Raspbery Pi 5 8GB with the M2 hat, and 512GB SSD
- You are free to use any lab setup that you like, but the course staff will only provide technical assistance for VMs and Virtualization software officially supported by the class. 
- See Course Documentation for a walkthrough.
- Remember, take lots of Snapshots. VMs can sometimes be bit unstable.
- For folks using the Raspberry pi 5 for the class, please store backups on a flash drive 
- Do not deviate from course directions 
- **story time** using the wrong version of mingw
---
<!-- slide template="[[Base Slide]]" -->
::: title
#### Technology 

:::

- For folks who are unable to purchase a Raspberry Pi, please come talk to me. 
- You can likely get away with using qemu for most assignments 


---
<!-- slide template="[[Base Slide]]" -->
::: title
#### More Elaborate Setups

:::

- This lab setup is not sufficient for a real world triage environment
- Depending on interest, we may cover other ways to improve the current setup
- For one, it is obviously a sandbox :)
- But, it should provide a reasonable amount of isolation and security. 
---
<!-- slide template="[[Base Slide]]" -->
::: title
#### Course Administrative Technology 

:::

- Students are required to join the class [[Discord]] server 
- As of now, all assignments are to be  submitted through GradeScope
	- this may change as I explore more virtualization/autograding tech
	- (I might migrate to GitHub classroom)
- Course notes, slides and assignment directions will use Obsidian.md 
- If you see a broken link in a github MD file, it is probably because you need to open it in Obsidian 

---
<!-- slide template="[[Base Slide]]" -->
::: title
#### [[Syllabus#PreReqs]]

:::

- Comfort with using Linux. Shells, text editors, configuration ...etc
- Combinatorics
- Probability (recommended) 
- Computing systems : familiarity with memory management, c programming, reading assembly, using debuggers and compiler toolchains. 
- Basic cryptography: familiarity with Ciphers, MACs and Hash functions. Ideally also the idea behind asymmetric cryptography
- Computer Networks:  (TCP/IP/UDP/HTTP/TLS)
---
<!-- slide template="[[Base Slide]]" -->
::: title
#### [[Syllabus#PreReqs]]

:::

- GDB/experience debugging  native binaries
- Experience with compiler toolchains (compilers, linkers)
- Wireshark is a plus 
- As is Ghidra or some other SRE framework

---
<!-- slide template="[[Base Slide]]" -->
::: title
#### Requirements: Programming/scripting

:::

- Python 3: you need to be very proficient with python. If you don't know python, Java is also OK but you will need to learn python :-)
- c/c++/some other systems programming language: you should be able to write simple C programs, know the difference between Stack and Heap memory. I.e. how to manage memory 
- intel x86 assembly: you should be able to read x86 assembly. (or some other assembly language) 
- Being able to write assembly is a plus!
- You should understand the basics of what an operating system is. 
---
<!-- slide template="[[Base Slide]]" -->
::: title
#### [[Syllabus#Protocols]] 

:::

- IP: you should know the difference between a private and public IP, the basics of addressing, and know the relationship between a domain name, DNS and an IP address
- TCP: More so the basics of socket programming, less so the internals of how reliability is achieved or what the handshake process looks like.
- DNS: You don't need to understand **how** DNS works, but you need to know what it does.
- TLS: You need to understand what a Certificate Authority is, a TLS cert is, and the basics of establishing secure tunnels 
- HTTP(s): HTTP verbs, request structure, and error codes


---
<!-- slide template="[[Base Slide]]" -->
::: title
#### Raspberry PI

:::

- Pi 5, 8GB
- m2 hat with 512GB storage 
- Ethernet Cable
- Pi 5 Power cable 
- micro sd card or nvme to USB adapter 
- Pi 5 Fan 
- See [[Syllabus#Hardware Requirements]] for details

---
<!-- slide template="[[Base Slide]]" -->
::: title
#### Requirements: Emulator (unsupported)

:::

- 16GB of RAM, a decent CPU, at least 100GB of storage 
	- If your computer is low on space, you can install the VM in an external SSD connected via USB3. You can find these at microcenter for around $50-100
- Intel/AMD based CPU.  
- Internet connection 

---
<!-- slide template="[[Base Slide]]" -->
::: title
#### Remarks about pre-requisite

:::

- Is Infosec for me?
-  Short answer: yes
-  Long answer: also yes 
- Do I have the right background for this class?
- Take a look at HW 0 (CPractice) 
- If you are lost, it might be a good idea to take this course next year! You are welcome to stick it out but you might need to put in extra work.


---
<!-- slide template="[[Base Slide]]" -->
::: title
#### Course Livestream		

:::

The course will be streamed via either Discord
- As of now, it looks like I will use OBS + Discord
- Bear with me as I learn how to live stream :-)
	- In particular, expect some recordings to get corrupted or the audio quality to be sub par. 
---
<!-- slide template="[[Base Slide]]" -->
::: title
#### Office Hours	

:::

- Non-deterministic. By appointment only.  
- Where is my office?
- Good question. I don’t know :D
	- For the time being, I will usually migrate to a coffee shop
---
<!-- slide template="[[Base Slide]]" -->
::: title
#### Bonus/Guest Lectures	

:::

- Experts from industry, some of which are hiring, will periodically swing by to give a bonus lecture
- You should come if you can make it!
- There will often be Pizza 
	- If you have a dietary restriction, let me know :-)




---
<!-- slide template="[[Title Slide]]" -->
 ## Questions?
---
<!-- slide template="[[Base Slide]]" -->
::: title
#### Homework

:::

- Join [[Discord]] 
- Join Gradescope 
- Join CTF
- C practice problems
- Purchase course hardware, and setup Lab Environment 
	- You will need  very good internet 
connection for this. 
	- Coffee shop internet will not cut it.
	- You should carve out some time in case you need to debug. 



---
<!-- slide template="[[Base Slide]]" -->
::: title
#### Homework (Continued)

:::

- The lab environment is complex to setup: especially for folks who haven't used virtual machines before!
- The first assignment will be released on the course GitHub 
- It will require a working Lab environment
- It may or may not have some Easter Eggs :egg:


---
<!-- slide template="[[Base Slide]]" -->
::: title
#### Discussion	

:::

- What is malware?