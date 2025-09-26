
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
- Kai 
- Security Researcher
- Lecturer + PHD candidate @ NEU
	- I have three emails and I check neither of them
	- Please ping me on discord 
	- if you send me an email expect some delay.
 
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

- The class introduces students to the wild world of offensive capabilities development 
- Students will take on the role of both developer and analyst by  attacking and tracking changes to a target environment.  
- Focusing on  Aarch64 Linux, students will create capabilities that are to be lawfully used against a simulated threat actor: APT CHONKY bear. The tools will primarily target the enigmatic CHONKY system. 
-  In particular,  students will also practice hacking by attempting to hack the hackers-- you will be writing and deploying malware on APT-Ch0nky Bear Infrastructure after identifying and exploiting the vulnerability in it.   




---
<!-- slide template="[[Base Slide]]" -->
::: title
#### Benefits of Learning to Write Malware  
:::

- Implant development is  esoteric OS development 
- It helps you master the ins and outs of an OS
- The first thing I do when learning about a new system/architecture is write a poc implant targeting it.
- usually this means writing a simple loader/beacon
- Loading code, and communicating to a remote C2
	- C2 Channels 
- TLDR  trojan horsed way of teaching Systems security :)

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
- "What would an Analyst/decision maker  care about?"
---
<!-- slide template="[[Split Vertical]]" -->
::: title
 #### Why get into Cybersecurity
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
- Get your hands dirty  writing purpose built malware
- You read that right. In this class you will be writing malware<!-- element class="fragment highlight-red" -->


---
<!-- slide template="[[Base Slide]]" -->

::: title

#### The Pieces 

:::
- IO: learn how Linux handles input and output 
- Payloads/Delivery: Learn about key file formats on Linux and how real exploits deliver payloads 
- Execution/Injection: learn how to execute code 

---
<!-- slide template="[[Split Vertical]]" -->
::: title

#### Become a NOC 

::: 
::: left 
- **FOR IMMEDIATE RELEASE** 
- Congrats: you've just joined the Network Observation Command (NOC) a (totally legit)  3 letter agency tasked with gaining insight into our nations adversaries in cyberspace.  
- Perks: cafeteria tacos,  a pension, opportunity to write memos in ALL CAPS for authenticity.
::: 
::: right
![[NOC_command.png]]

:::

---
<!-- slide template="[[Base Slide]]" -->
::: title
#### [[MalwareCourse/Capstone]] 

:::
- As a member of the elite NOC force, your jobs is to  gather information about the notorious APT-CHONKY-BEAR
- This course is broken into 3 parts each section culminating with  students deploying a capability against CHONKY-BEAR 
- The course capstone is to  implement a production ready Remote Administration Tool (RAT)  targeting a bespoke target with unique intelligence requirements .
- The implant will be detonated on a raspberry pi 5 (8gb) running  ubuntu 
- You have to work in a groups for the final capstone project.
<b><center>** Infosec is a Team sport!**</center> </b> 
- Go make friends with your neighbor 
![[infosec_is_teamsport]]
---
<!-- slide template="[[Split Vertical]]" -->
::: title
#### Capstone Progression
:::

::: left
- Each section of the course begins with a brief on a new APT-CHONKY-BEAR target
- In order  exploit the system, you will need to develop a capability 
- With each new iteration, you will build out more components of your capability culminating in an implant purpose built for a target system. 
:::
::: right
![[Ch0nkyBear.png|450]]
:::

---
<!-- slide template="[[Title Slide]]" -->
 ### This is a new field. All concepts learned in this class are directly applicable to industry. 

---
<!-- slide template="[[Base Slide]]" -->
::: title
#### Course Success Stories	

:::

For students who demonstrate passion for the subject, I will personally leverage my network to help you find a job

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
- TLDR: C, Armv8-A, Linux (mostly userland) Internals, Ghidra, implant dev
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
![[new_class_new_year.png]]

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
