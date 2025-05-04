#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "userprog/process.h"
#include "threads/vaddr.h"
#include "filesys/filesys.h"
#include "filesys/file.h"
#include "devices/shutdown.h"
#include "devices/input.h"
#include <string.h>
#include <stdlib.h>
#include "syscall.h"
#include "threads/synch.h"

// Global lock used to prevent race conditions when accessing files.
static struct lock file_lock;

static void syscall_handler(struct intr_frame *);
void validate_address(const void *address);

void halt(void);
void exit(int status);
tid_t exec(char *cmd_line);
int wait(int pid);

bool create (const char *file, unsigned initial_size);
bool remove (const char *file);
int open (char *file);
int filesize (int fd);
int read (int fd, void *buffer, unsigned size);
int write(int fd, void *buffer, int length);
void seek (int fd, unsigned position);
unsigned tell (int fd);
void close (int fd);
struct opened_file *fd2file(int fd);

void syscall_init(void){
  intr_register_int(0x30, 3, INTR_ON, syscall_handler, "syscall");
  lock_init(&file_lock);
}

// Ensures that the given address is not NULL, within the user address space,
// and maps to a valid page in memory.
void validate_address(const void *address) {
    if (address == NULL || !is_user_vaddr(address) || pagedir_get_page(thread_current()->pagedir, address) == NULL) {
        exit(-1);
    }
}


// Handle system calls
static void syscall_handler(struct intr_frame *f) {
    // Validate the address of the stack pointer
    validate_address(f->esp);

    // Store the stack pointer in a local variable
    void *esp = f->esp;
    int *args = (int *)malloc(3 * sizeof(int)); // Allocate memory for arguments
	

    // Declare variables for system call parameters
    int fd;
    void *buffer;
    int size;
    char *file;
    
    // Identify the system call and execute the corresponding functionality
    //printf("System call: %d\n", *(int *)esp);

    switch (*(int *)esp) {
      case SYS_HALT: // No parameters 
          shutdown_power_off();
          break;
      case SYS_EXEC: // One parameter
        validate_address(esp + 4);
        char *cmd_line = (char *)(*((int *)esp + 1));
        if (cmd_line == NULL)
            exit(-1);
        lock_acquire(&file_lock);
        f->eax = exec(cmd_line);
        lock_release(&file_lock);
        break;
      
      case SYS_WRITE:
        validate_address(esp + 4);
        validate_address(esp + 8);
        validate_address(esp + 12);
        fd = *((int *)f->esp + 1);
        buffer = (void *)(*((int *)f->esp + 2));
        size = *((int *)f->esp + 3);

        if (buffer == NULL || size < 0) {
            exit(-1);
        }
        f->eax = write(fd, buffer, size);
        
        break;
      
      case SYS_OPEN:
        validate_address(esp + 4);
        char *file_name = (char *)(*((uint32_t *)esp + 1));
        if (file_name == NULL)
            exit(-1);
        f->eax = open(file_name);
        break;
      default:
        // Handle other system calls here
        printf("System call: %d\n", *(int *)esp);
        break;       
    }
}

tid_t exec(char *cmd_line) {
    
    return process_execute(cmd_line);
}

void exit(int status){
    struct thread *cur = thread_current();
    cur->exit_status = status;
    printf("%s: exit(%d)\n", cur->name, status);
    thread_exit();
}
int write(int fd, void *buffer, int length){

    if (fd == 1){
        lock_acquire(&file_lock);
        putbuf(buffer, length);
        int sizeActual = length;
        lock_release(&file_lock);
        return sizeActual;
    }
    printf("<---------------------------- fd11 = %d ---------------------------->\n", fd);
    struct file *file = fd2file(fd)->ptr;
    printf("<---------------------------- fd = %d ---------------------------->\n", fd);
    lock_acquire(&file_lock);
    if (file == NULL)
        return -1;
    int sizeActual = (int)file_write(file, buffer, length);
    lock_release(&file_lock);
    return sizeActual;
}

int open(char *file_name)
{
  struct opened_file *open = palloc_get_page(0);
  if (open == NULL){
      palloc_free_page(open);
      return -1;
  }
  lock_acquire(&file_lock);
  open->ptr = filesys_open(file_name);
  printf("<---------------------------- fd = %s ---------------------------->\n", file_name);
  lock_release(&file_lock);
  if (open->ptr == NULL)
      exit(-1);
  thread_current()->fileDirectory++;
  open->fd = thread_current()->fileDirectory;
  list_push_back(&thread_current()->file_list, &open->elem);
  return open->fd;

}

struct opened_file *fd2file(int fd){
  struct thread *t = thread_current();
  for (struct list_elem *e = list_begin(&t->file_list); e != list_end(&t->file_list);e = list_next(e)){
      struct opened_file *opened = list_entry(e, struct opened_file, elem);
      if (opened->fd == fd)
          return opened;
  }
  return NULL;
}


