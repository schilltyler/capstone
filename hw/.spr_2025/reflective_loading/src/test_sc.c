
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // For memcpy (though fread can read directly)

// Platform-specific includes for memory allocation
#include <errno.h>
#include <sys/mman.h>
#include <unistd.h> // For sysconf(_SC_PAGESIZE) - though not strictly needed for this mmap use

int main(int argc, char *argv[]) {
  FILE *file = NULL;
  unsigned char *rwx_mem = NULL; // Use unsigned char* for byte-level access
  long file_size = -1;
  size_t bytes_read = 0;

  // --- Argument Check ---
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <executable_file>\n", argv[0]);
    return EXIT_FAILURE;
  }
  const char *filename = argv[1];
  printf("Attempting to load and execute: %s\n", filename);

  // --- Open File (Binary Read Mode) ---
  file = fopen(filename, "rb");
  if (!file) {
    perror("Error opening file");
    return EXIT_FAILURE;
  }

  // --- Get File Size ---
  if (fseek(file, 0, SEEK_END) != 0) {
    perror("Error seeking to end of file");
    fclose(file);
    return EXIT_FAILURE;
  }
  file_size = ftell(file);
  if (file_size < 0) {
    perror("Error getting file size");
    fclose(file);
    return EXIT_FAILURE;
  }
  if (file_size == 0) {
    fprintf(stderr, "Error: Input file is empty.\n");
    fclose(file);
    return EXIT_FAILURE;
  }
  // Rewind to the beginning of the file
  rewind(file);
  printf("File size: %ld bytes\n", file_size);

  // --- Allocate RWX Memory ---
  printf("Allocating %ld bytes of RWX memory...\n", file_size);
  // POSIX: mmap
  rwx_mem = (unsigned char *)mmap(
      NULL,                               // Let kernel choose address
      file_size,                          // Length of mapping
      PROT_READ | PROT_WRITE | PROT_EXEC, // Protection flags (RWX)
      MAP_PRIVATE | MAP_ANONYMOUS,        // Private, anonymous mapping
      -1,                                 // File descriptor (-1 for anonymous)
      0                                   // Offset
  );
  if (rwx_mem == MAP_FAILED) {
    perror("Error allocating RWX memory with mmap");
    rwx_mem = NULL; // Ensure pointer is NULL on failure
    fclose(file);
    return EXIT_FAILURE;
  }
  printf("RWX Memory allocated at address: %p\n", (void *)rwx_mem);

  // --- Read File Content into RWX Memory ---
  printf("Reading file content into RWX memory...\n");
  bytes_read = fread(rwx_mem, 1, file_size, file);
  if (bytes_read != (size_t)file_size) {
    if (feof(file)) {
      fprintf(stderr,
              "Error reading file: Unexpected end of file (read %zu, expected "
              "%ld).\n",
              bytes_read, file_size);
    } else if (ferror(file)) {
      perror("Error reading file");
    } else {
      fprintf(
          stderr,
          "Error reading file: Unknown read error (read %zu, expected %ld).\n",
          bytes_read, file_size);
    }

    // Cleanup allocated memory before exiting
    munmap(rwx_mem, file_size);
    fclose(file);
    return EXIT_FAILURE;
  }
  fclose(file); // Close the file handle now that we're done with it
  file = NULL;

  // --- Execute the Code ---
  printf("Executing code at %p...\n", (void *)rwx_mem);

  // Create a function pointer to the beginning of the RWX memory
  // We assume the code doesn't take arguments and doesn't return (or we ignore
  // return)
  sleep(0);
  void (*executable_code)(void) = (void (*)(void))rwx_mem;

  // Call the function pointer - this jumps to the code in rwx_mem
  executable_code();

  // --- Code after execution ---
  // This part will likely NOT be reached if the executed code calls exit() or
  // crashes. It will only be reached if the shellcode/program explicitly
  // returns.
  printf("Execution finished (code returned).\n");

  // --- Cleanup (if execution returns) ---
  printf("Cleaning up allocated memory...\n");
  munmap(rwx_mem, file_size);
  rwx_mem = NULL;

  printf("Program finished normally.\n");

  return EXIT_SUCCESS;
}
