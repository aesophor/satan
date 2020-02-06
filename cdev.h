// Copyright (c) 2020 Marco Wang <m.aesophor@gmail.com>
#ifndef SATAN_CDEV_H_
#define SATAN_CDEV_H_

#include <linux/module.h>

int satan_cdev_init(struct module *m);
void satan_cdev_exit(void);

#endif  // SATAN_CDEV_H_
