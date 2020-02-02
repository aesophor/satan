// Copyright (c) 2020 Marco Wang <m.aesophor@gmail.com>
#ifndef SATAN_HIDE_H_
#define SATAN_HIDE_H_

#include <linux/types.h>

bool satan_is_hidden(void);
void satan_set_hidden(bool hidden);

#endif  // SATAN_HIDE_H_
