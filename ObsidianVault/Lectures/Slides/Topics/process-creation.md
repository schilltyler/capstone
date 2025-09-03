
---
<!-- slide template="[[Base Slide]]" -->

::: title

#### From file bytes to Process

:::
- fo




---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Kernel's Knowedge of ELF 

:::
- minimal, only needs to memory map segments
- setup basic scratch memory  

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Simple  case: Statically Linked binary  

:::
- foo bar


---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Dynamically Linked Binaries  

:::
- Inteprreter: ld.so


---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Handling Relocations 

:::
-  GOT



---
<!-- slide template="[[Base Slide]]" -->

::: title

#### PLT 

:::
- Procedure Linkage table
- lazily resolved 
- `dlopen/dlsym`



---
<!-- slide template="[[Base Slide]]" -->

::: title

#### BSS 

:::
- foo bar

---
<!-- slide template="[[Base Slide]]" -->

::: title

#### TLS 

:::
- foo bar


---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Dynamic Linking 

:::
- `man ld.so`
- interpreter --> runtime linker 



---
<!-- slide template="[[Base Slide]]" -->

::: title

#### Title

:::
- foo bar