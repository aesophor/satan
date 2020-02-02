// Copyright (c) 2020 Marco Wang <m.aesophor@gmail.com>
#ifndef SATAN_DEV_H_
#define SATAN_DEV_H_

#include <linux/module.h>
#include <linux/semaphore.h>

struct satan_device {
        char data[100];
        struct semaphore smphore;
};


int satan_dev_init(struct module *m);
void satan_dev_destroy(void);

#endif  // SATAN_DEV_H_
