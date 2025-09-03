
## 1. String Reversal Using Pointer Arithmetic

**Description:**  
Write a function to reverse a string **in place** using pointer arithmetic instead of array indexing.

**Requirements:**

1. Implement a function `void reverse_string(char *str)` that:
    - Takes a C-string as input.
    - Reverses the string **in place** using only pointer operations (no `str[len - i - 1]` indexing).
2. Consider edge cases such as empty strings and strings of length 1.

**Example Usage:**

```c
char myStr[] = "Hello";
reverse_string(myStr);
printf("%s\n", myStr); // Should print "olleH"
```

**Test Cases:**

1. `""` (empty string) -> `""`
2. `"H"` -> `"H"`
3. `"Hello"` -> `"olleH"`
4. `"ABCDE"` -> `"EDCBA"`

---

## 2. Finding the Maximum in an Integer Array Using Pointer Arithmetic

**Description:**  
Write a function to find the maximum element in an integer array using pointer arithmetic.

**Requirements:**

1. Implement a function `int find_max(int *arr, size_t len)` that:
    - Returns the largest integer in the array.
    - Uses pointer arithmetic instead of `arr[i]` indexing.
2. Handle the case when `len == 0` (either return a sentinel value or handle it gracefully).

**Example Usage:**

```c
int nums[] = {5, 3, 9, 1, 9, 7};
int maxVal = find_max(nums, 6);
printf("%d\n", maxVal); // Should print "9"
```

**Test Cases:**

1. `[]` (empty array) -> handle gracefully (e.g., return `INT_MIN`).
2. `[5]` -> `5`
3. `[5, 3, 9, 1, 9, 7]` -> `9`
4. `[-1, -5, -3]` -> `-1`

---

## 3. Dynamic Array Allocation and Summation

**Description:**  
Write a program that reads an integer `N` from the user, allocates memory to store `N` integers, reads them from stdin, and calculates their sum.

**Requirements:**

1. Prompt the user for `N`.
2. Allocate memory using `malloc()` for `N` integers.
3. Read `N` integers and store them in the allocated array.
4. Compute the sum of these integers.
5. Print the result and free the allocated memory.

**Example Interaction:**

```
Enter number of elements: 4
Enter 4 integers: 10 5 2 3
Sum = 20
```

**Test Cases:**

1. `N = 0` -> sum is `0`; no memory is allocated.
2. `N = 1` with input `[42]` -> sum is `42`.
3. `N = 5` with input `[1, 2, 3, 4, 5]` -> sum is `15`.

---

## 4. Creating and Freeing a 2D Dynamic Array

**Description:**  
Write functions to create, populate, and free a 2D array dynamically.

**Requirements:**

1. Implement `int** create_2d_array(size_t rows, size_t cols)`.
    - Use `malloc()` or `calloc()` to allocate a 2D array of integers.
2. Implement `void free_2d_array(int **arr, size_t rows)`.
    - Properly free all allocated memory.
3. In `main()`, demonstrate usage by creating a 2D array, populating it with a pattern, printing it, then freeing it.

**Example Usage:**

```c
int main(void) {
    size_t rows = 3, cols = 4;
    int **arr = create_2d_array(rows, cols);
    
    // Populate the array, e.g., arr[i][j] = i * cols + j
    // Print the array
    // ...
    
    free_2d_array(arr, rows);
    return 0;
}
```

**Test Cases (Verification):**

1. `rows = 0` or `cols = 0` -> handle gracefully (no allocation or partial allocation).
2. Small 2D array (e.g. `3 x 4`) -> verify values are stored and retrieved correctly.
3. Larger 2D array (e.g. `10 x 10`) -> check for memory leaks (using Valgrind or similar).

---

## 5. Stack vs. Heap Variables

**Description:**  
Write a short program demonstrating the difference between a stack variable (local array) and a dynamically allocated array (heap).

**Requirements:**

1. In `main()`, create:
    - A local array of fixed size (stack).
    - A dynamically allocated array of the same size (heap).
2. Print addresses and show how they differ. For example:
    
    ```c
    printf("Stack array address: %p\n", (void*)stackArray);
    printf("Heap array address: %p\n", (void*)heapArray);
    ```
    
3. Demonstrate that the stack variable’s lifetime ends at function exit, while the heap variable persists until explicitly freed.

**Test Cases (Verification):**

- Run and observe output. Stack memory addresses are often closer to each other or in a specific segment, while heap addresses differ significantly.
- Confirm no memory leaks (ensure you call `free(heapArray)`).

---

## 6. Managing a `struct` for Complex Numbers

**Description:**  
Define a `struct complex` that holds two `double` values: `real` and `imag`. Write functions to create, add, and print complex numbers.

**Requirements:**

1. Define:
    
    ```c
    typedef struct {
        double real;
        double imag;
    } complex;
    ```
    
2. Implement:
    - `complex create_complex(double real, double imag)`
    - `complex add_complex(complex c1, complex c2)`
    - `void print_complex(complex c)`
3. In `main()`, demonstrate creating and adding complex numbers.

**Example Usage:**

```c
complex c1 = create_complex(1.0, 2.0);
complex c2 = create_complex(3.0, 4.0);
complex result = add_complex(c1, c2);
// Should be (4.0, 6.0)
print_complex(result);
```

**Test Cases:**

1. `(0, 0) + (0, 0) = (0, 0)`
2. `(1, 2) + (3, 4) = (4, 6)`
3. `(-1, 5) + (2, -5) = (1, 0)`

---

## 7. Managing a `struct` for Student Records

**Description:**  
Define a `struct Student` to store each student’s name, ID, and GPA. Then write a program to read and store multiple students dynamically.

**Requirements:**

1. Define:
    
    ```c
    typedef struct {
        char name[50];
        int id;
        float gpa;
    } Student;
    ```
    
2. Prompt the user for the number of students, `N`.
3. Dynamically allocate an array of `Student` of size `N`.
4. For each student, read `name`, `id`, and `gpa` from stdin.
5. Print the data for all students and then free the allocated memory.

**Example Interaction:**

```
Number of students: 2
Student 1: Name, ID, GPA
John Doe 1234 3.4
Student 2: Name, ID, GPA
Jane Smith 5678 3.8

--- Student Records ---
Name: John Doe, ID: 1234, GPA: 3.40
Name: Jane Smith, ID: 5678, GPA: 3.80
```

**Test Cases:**

1. `N = 0` -> no students, no allocation.
2. `N = 1` -> single student, verify correct reading and printing.
3. Multiple students, check boundary conditions for `name` length, ID range, etc.

---

## 8. Using POSIX APIs to Read a File

**Description:**  
Use POSIX file APIs (`open`, `read`, `close`) to read the contents of a text file and print it to stdout.

**Requirements:**

1. The program should take the file path as a command-line argument.
2. Use `int fd = open(filename, O_RDONLY);` to open the file.
3. Use `read(fd, buffer, bufSize);` in a loop to read chunks of data.
4. Print the data to stdout (e.g., with `write(STDOUT_FILENO, buffer, bytesRead);` or `printf`).
5. Close the file using `close(fd);`.
6. Handle errors (e.g., file not found).

**Example Invocation:**

```bash
./readfile sample.txt
```

**Test Cases:**

1. A small file (e.g., `Hello World`).
2. A larger file (multiple lines).
3. Non-existent file -> check error handling.

---

## 9. Using POSIX APIs to Write to a File

**Description:**  
Write a program that takes two command-line arguments: an output filename and a string to write into that file.

**Requirements:**

1. Open/Create the file in write-only mode (`O_WRONLY | O_CREAT | O_TRUNC`).
2. Use `write(fd, str, strlen(str));` to write the string to the file.
3. Close the file.
4. Handle errors (e.g., unable to create file).

**Example Invocation:**

```bash
./writefile output.txt "Hello, world!"
```

**Test Cases:**

1. Write a short string to a new file -> verify contents.
2. Write to an existing file -> file should be overwritten.
3. Handle cases where user does not provide enough arguments.

---

## 10. Self-Contained Program Demonstrating File Copy

**Description:**  
Combine reading and writing into a single program that copies a file, using only POSIX APIs (`open`, `read`, `write`, `close`).

**Requirements:**

1. Command-line usage: `./filecopy source_file dest_file`
2. Open `source_file` for reading.
3. Open or create `dest_file` for writing (truncate if it exists).
4. Read chunks of data from source and write to destination in a loop.
5. Close both files.
6. Handle errors (file not found, permission denied, etc.).

**Example Invocation:**

```bash
./filecopy original.txt backup.txt
```

**Test Cases:**

1. Copy an empty file -> destination should also be empty.
2. Copy a small text file -> verify contents are identical.
3. Copy a large file -> verify correct copy (checksum or compare).


