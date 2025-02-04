
---
<!-- slide template="[[Base Slide]]" -->
::: title
#### Where to Start? (ELF Binaries)
:::

1. **Hash** the binary (e.g., `sha256sum`, `md5sum`). Look it up in internal DB or external threat intel (careful with VirusTotal!).  
2. **Strings**: Look for text references in the binary (`strings`, Ghidra’s string search, `radare2 -z`).  
3. **Imported Symbols**: Check which library calls the ELF references (using `readelf -Ws` or `objdump -T`).  
4. **Resources / Embedded Data**: Check any suspicious large data blocks or extra sections.  `man binwalk` 
5. **Entry Point** → check `readelf -h` or open in Ghidra for assembly.  
6. **Decompiled View / X-refs**: Let Ghidra/radare2 help you see higher-level logic.
