// Copyright (c) 2020 Marco Wang <m.aesophor@gmail.com>
#ifndef SATAN_DEV_H_
#define SATAN_DEV_H_

#include <linux/module.h>

int satan_dev_init(struct module *m);
void satan_dev_exit(void);

#endif  // SATAN_DEV_H_
