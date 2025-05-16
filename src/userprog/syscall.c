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

int exit(int status);

static void syscall_handler (struct intr_frame *);

static struct lock lock;

is_valid_address(const void *t){
    if (t == NULL || !is_user_vaddr(t) || pagedir_get_page(thread_current()->pagedir, t) == NULL)
        exit(-1);
}

void
syscall_init (void) 
{
    intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
    lock_init(&lock);
}
static void
syscall_handler (struct intr_frame *f UNUSED) 
{
    printf ("system call!\n");
    is_valid_address(f->esp);
    void *esp = f->esp;
    int fd;
    void *buffer;
    int size;
    char *file;
    if(*(int *)esp == SYS_HALT) shutdown_power_off();
        
    else if(*(int *)esp == SYS_EXIT) {
        is_valid_address(esp + 4);
        int status = *((int *)esp + 1);
        exit(status);
        
    }
    else if(*(int *)esp == SYS_EXEC) {
        is_valid_address(esp + 4);
        char *cmd_line = (char *)(*((int *)esp + 1));
        if (cmd_line == NULL)
            exit(-1);
        lock_acquire(&lock);
        //f->eax = exec(cmd_line);
        lock_release(&lock);
    }
    else if(*(int *)esp == SYS_WAIT) {
        is_valid_address(esp + 4);
        int pid = (*((int *)esp + 1));
        //f->eax = wait(pid);
    }
    else if(*(int *)esp == SYS_CREATE) {
        is_valid_address(esp + 4);
        is_valid_address(esp + 8);
        file = (char *)(*((uint32_t *)esp + 1));
        unsigned init_size = *((unsigned *)esp + 2);
        if (file == NULL)
            exit(-1);
        //f->eax = create(file, init_size);
    }
    else if(*(int *)esp == SYS_REMOVE) {
        is_valid_address(esp + 4);
        file = (char *)(*((uint32_t *)esp + 1));
        if (file == NULL)
            exit(-1);
        //f->eax = remove(file);
    }
    else if(*(int *)esp == SYS_OPEN) {
        is_valid_address(esp + 4);
        char *file_name = (char *)(*((uint32_t *)esp + 1));
        if (file_name == NULL)
            exit(-1);
        //f->eax = open(file_name);
    }
    else if(*(int *)esp == SYS_FILESIZE) {
        is_valid_address(esp + 4);
        fd = *((uint32_t *)esp + 1);
        //f->eax = fileSize(fd);
    }
    else if(*(int *)esp == SYS_READ) {
        is_valid_address(esp + 4);
        is_valid_address(esp + 8);
        is_valid_address(esp + 12);

        fd = *((int *)f->esp + 1);
        buffer = (void *)(*((int *)f->esp + 2));
        size = *((int *)f->esp + 3);
        is_valid_address(buffer + size);
        //f->eax = read(fd, buffer, size);
    }
    else if(*(int *)esp == SYS_WRITE) {
        is_valid_address(esp + 4);
        is_valid_address(esp + 8);
        is_valid_address(esp + 12);
        fd = *((uint32_t *)esp + 1);
        buffer = (void *)(*((uint32_t *)esp + 2));
        size = *((unsigned *)esp + 3);
        if (buffer == NULL)
            exit(-1);
        //f->eax = write(fd, buffer, size);
    }
    else if(*(int *)esp == SYS_SEEK) {
        is_valid_address(esp + 4);
        is_valid_address(esp + 8);
        fd = *((uint32_t *)esp + 1);
        int pos = (*((unsigned *)esp + 2));
        //seek(fd, pos);
    }
    else if(*(int *)esp == SYS_TELL) {
        is_valid_address(esp + 4);
        fd = *((uint32_t *)esp + 1);
        //f->eax = tell(fd);
    }
    else if(*(int *)esp == SYS_CLOSE) {
        is_valid_address(esp + 4);
        fd = *((uint32_t *)esp + 1);
        //close(fd);
    }
  //thread_exit ();
}

int exit(int status) {
    return status;
}
