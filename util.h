// Copyright (c) 2020 Marco Wang <m.aesophor@gmail.com>
#ifndef SATAN_UTIL_H_
#define SATAN_UTIL_H_

#define CR0_WP_DISABLE                                             \
        do {                                                       \
                pr_info("satan: disabling cr0 write protection");  \
                write_cr0(read_cr0() & (~ 0x10000));               \
        } while (0);

#define CR0_WP_DISABLE_END                                         \
        do {                                                       \
                pr_info("satan: enabling cr0 write protection");   \
                write_cr0(read_cr0() | 0x10000);                   \
        } while (0);


void satan_basename(const char *path, char *buf, size_t buf_size);
void satan_filename(const char *path, char *buf, size_t buf_size);

#endif  // SATAN_UTIL_H_
