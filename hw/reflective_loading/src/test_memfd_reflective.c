#include <elf.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <path_to_reflective.so>\n", argv[0]);
    return 1;
  }
  const char *so_path = argv[1];
  // 1. Open the shared object file
  int fd = open(so_path, O_RDONLY);
  if (fd < 0) {
    perror("open");
    return 1;
  }
  struct stat st;
  if (fstat(fd, &st) < 0) {
    perror("fstat");
    close(fd);
    return 1;
  }
  size_t so_size = st.st_size;

  // 2. Allocate a RWX memory region to hold a copy of the shared object
  void *mapping = mmap(NULL, so_size, PROT_READ | PROT_WRITE | PROT_EXEC,
                       MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (mapping == MAP_FAILED) {
    perror("mmap");
    close(fd);
    return 1;
  }

  // 3. Read the entire file into the allocated memory
  ssize_t rd = read(fd, mapping, so_size);
  if (rd < 0 || (size_t)rd < so_size) {
    perror("read");
    munmap(mapping, so_size);
    close(fd);
    return 1;
  }
  close(fd);

  // 4. Locate the file offset of the `load_refl` function
  // We use the ELF symbol table to find the symbol "load_refl"
  Elf64_Ehdr *eh = (Elf64_Ehdr *)mapping;
  size_t func_offset = 0;
  func_offset = eh->e_entry;
  if (func_offset == 0) {
    fprintf(stderr, "Entrypoint is null: %s\n", so_path);
    munmap(mapping, so_size);
    return 1;
  }

  // 5. Jump to the function in the mapped memory
  printf("[*] Loader: Jumping to load_refl function at offset 0x%zx\n",
         func_offset);

  uintptr_t (*func_ptr)(size_t) =
      (uintptr_t(*)(size_t))((char *)mapping + func_offset);
  sleep(0);
  uintptr_t result = func_ptr(so_size);
  printf("[*] Loader: load_refl returned %p\n", (void *)result);
  printf("Nice\n");
  // 6. (Optional) The new shared object is loaded in memory. In a real
  // use-case, we could now use it. For this demonstration, we simply unload the
  // memory and exit.
  munmap(mapping, so_size);
  printf("[+] Gracefuly exiting now!\n");
  return 0;
}
