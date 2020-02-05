// Copyright (c) 2020 Marco Wang <m.aesophor@gmail.com>
#ifndef SATAN_SYSCALL_H_
#define SATAN_SYSCALL_H_

int satan_syscall_init(void);
int satan_syscall_exit(void);

int satan_syscall_hook(int id, void *hook);
int satan_syscall_unhook(int id);
unsigned long *satan_syscall_get_original(int id);

#endif  // SATAN_SYSCALL_H_
