#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "userprog/pagedir.h"
#include "userprog/process.h"
#include "userprog/syscall.h"

static void syscall_handler (struct intr_frame *);
int getpage_ptr(const void *vaddr);
void get_args (struct intr_frame *f, int *args, int num_of_args);
int validate_ptr (const void *vaddr);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  int syscall_number = 0;
  validate_ptr((const void*) f -> esp);
    int args[3];
    syscall_number = *( (int *) f -> esp);
    

  printf("esp: %d\n", syscall_number);

  if(syscall_number == SYS_EXEC){
    get_args(f, args, 1);

    printf("arg1: %d\n", args[0]); 
    printf("exec syscall\n");
    process_execute((const char *) args[0]);

  }
  else if(syscall_number == SYS_WRITE){
    get_args(f, args, 3);
    printf("arg1: %d\n", args[0]);
    printf("arg2: %d\n", args[1]);
    printf("arg3: %d\n", args[2]);
    printf("write syscall\n");
  }

  printf ("system call!\n");
  thread_exit ();
}



int getpage_ptr(const void *vaddr)
{
  void *ptr = pagedir_get_page(thread_current()->pagedir, vaddr);
  if (!ptr)
  {
    return -1;
  }
  return (int)ptr;
}

void get_args (struct intr_frame *f, int *args, int num_of_args)
{
  int i;
  int *ptr;
  for (i = 0; i < num_of_args; i++)
  {
    ptr = (int *) f->esp + i + 1;
    validate_ptr((const void *) ptr);
    args[i] = *ptr;
  }
}

int validate_ptr (const void *vaddr)
{
    if (vaddr < ((void *) 0x08048000) || !is_user_vaddr(vaddr))
    {
      // virtual memory address is not reserved for us (out of bound)
      return -1;
    }
    return 0;
}



