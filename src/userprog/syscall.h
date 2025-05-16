#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

void syscall_init (void);

void is_valid_address(const void* ptr); 

void exit(int status);

#endif /* userprog/syscall.h */
