// Copyright (c) 2020 Marco Wang <m.aesophor@gmail.com>
#include "util.h"

#include <linux/string.h>


/**
 * satan_basename() - Extracts the basename (dirname) from a path.
 * @path: absolute path
 * @buf: the buffer into which the extracted basename will be placed.
 * @buf_size: the size of `buffer` in bytes.
 */
void satan_basename(const char *path, char *buf, size_t buf_size)
{
        char *last_delim = NULL;

        if (strlen(path) >= buf_size) {
                pr_error("satan: util: buffer not large enough!\n");
                return;
        }

        strncpy(buf, path, strlen(path));
        last_delim = strrchr(buf, '/');

        if (!last_delim) {
                pr_error("satan: util: invalid path: %s\n", path);
                return;
        }

        *last_delim = 0;
}

/**
 * satan_filename() - Extracts the filename from a path.
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
                pr_error("satan: util: invalid path: %s\n", path);
                return;
        }

        filename = last_delim + 1;
        filename_len = strlen(filename);

        if (filename_len >= buf_size) {
                pr_error("satan: util: buffer not large enough!\n");
                return;
        }

        strncpy(buf, filename, strlen(filename));
        buf[buf_size - 1] = 0;
}
