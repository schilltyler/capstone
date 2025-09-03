


https://github.com/openbsd/src/blob/2347e6edcd5e06bc97a8ea95077c54fc76d2e634/libexec/ld.so/aarch64/archdep.h#L41

```

#ifndef _AARCH64_ARCHDEP_H_
#define _AARCH64_ARCHDEP_H_

#define	RELOC_TAG	DT_RELA
#define	MACHID		EM_AARCH64	/* ELF e_machine ID value checked */

/* Only used in lib/csu/boot.h */
#ifdef RCRT0

static inline void
RELOC_DYN(Elf_RelA *r, const Elf_Sym *s, Elf_Addr *p, unsigned long v)
{
	if (ELF_R_TYPE(r->r_info) == R_AARCH64_RELATIVE) {
		*p = v + r->r_addend;
	} else if (ELF_R_TYPE(r->r_info) == R_AARCH64_GLOB_DAT) {
		*p = v + s->st_value + r->r_addend;
	} else if (ELF_R_TYPE(r->r_info) == R_AARCH64_ABS64) {
		*p = v + s->st_value + r->r_addend;
	} else {
		_csu_abort();
	}
}

#endif /* RCRT0 */
#endif /* _AARCH64_ARCHDEP_H_ */
```



https://github.com/akawashiro/glibc/blob/008003dc6e83439c5e04a744b7fd8197df19096e/sysdeps/aarch64/dl-machine.h#L393-L425

