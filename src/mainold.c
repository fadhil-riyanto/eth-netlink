// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2025
 * Fadhil Riyanto <fadhil.riyanto@gnuweeb.org>
 */

#include <string.h>
#include <errno.h>
#include <linux/netlink.h>
#include <linux/genetlink.h>
#include <stdio.h>
#include "vt_hexdump.h"
#include <getopt.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <libmnl/libmnl.h>

#define RSP_BUFFER_SIZE 1024

struct option opt[] = {
        {"list-interfaces", no_argument, 0, 0x00},
        {"interface", required_argument, 0, 0x01},
        {"info", no_argument, 0, 0x02},
        {0, 0, 0, 0}
};

/* when --interface is present */
struct control_opt {
        char            cur_info;
        char            cur_change;
};

/* when --interface is not present */
struct control_opt_noiif {
        char            list_interfaces;
};

struct ctx {
        char                    carry_spesific_interface;
        struct control_opt*     control_opt;
        struct control_opt_noiif* control_opt_noiif;
};

/**
 * this is netlink message that will be send
 */
struct nl_fmt {
        struct nlmsghdr nlmsghdr;
        struct genlmsghdr genlmsghdr;
        struct nlattr nlattr;
        char* data;

        char* pad;
};

static void open_nl_socket() {
        int fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_GENERIC);

        if (fd <= -1) {
                printf("socket AF_NETLINK error, errno: ", errno);
                goto exit;
        }

        struct nlmsghdr netlink_message_header = {
                .nlmsg_len = 32,
                .nlmsg_type = ,
                .nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK,
                .nlmsg_seq = 1,
                .nlmsg_pid = 0
        };

        printf("sizeof : %zu\n", sizeof(netlink_message_header));

        struct genlmsghdr genlmsghdr = {
                .cmd = CTRL_CMD_GETFAMILY,
                .version = 2,
                .reserved = 0
        };

        struct nlattr nlattr = {
                .nla_len = 10,
                .nla_type = CTRL_ATTR_FAMILY_NAME,
        };

        struct nl_fmt fmt;
        fmt.nlmsghdr = netlink_message_header;
        fmt.genlmsghdr = genlmsghdr;
        fmt.nlattr = nlattr;
        fmt.data = "test1";
        fmt.pad = "\0\0";
        

        send(fd, &fmt, sizeof(struct nl_fmt), 0);

        char* buf = (char*)malloc(RSP_BUFFER_SIZE);
        memset(buf, 1024, 1);
        int n = recv(fd, buf, RSP_BUFFER_SIZE, 0);

        printf("received %d\n", n);

        VT_HEXDUMP(buf, RSP_BUFFER_SIZE);
        close(fd);

        // struct nlmsghdr nlmsghdr_res = 
        free(buf);
exit:
        return;
}

static void ethnetlink_main(struct ctx* ctx) {
        if (ctx->carry_spesific_interface == 0) {
                if (ctx->control_opt_noiif->list_interfaces == 1) {
                        printf("executing list interface\n");
                        open_nl_socket();
                }
        }
}

static void show_general_help() {
        printf("use ./eth-netlink <IFACE>\n");
}

static void parse_opt(int argc, char *argv[], struct ctx* ctx) {
        int optind = 1;
        int c;

        while (1) {
                c = getopt_long(argc, argv, "c:", opt, &optind);

                if (c == -1)
                        break;

                switch (c) {
                        case 0x00:
                        ctx->carry_spesific_interface = 0;
                        ctx->control_opt_noiif->list_interfaces = 1;
                        return;
                        break;
                }
        }
}

static struct ctx* init_all_memctx() {
        struct ctx* ctx = (struct ctx*)malloc(sizeof(struct ctx));
        ctx->control_opt = (struct control_opt*)malloc(sizeof(struct control_opt));
        ctx->control_opt_noiif = (struct control_opt_noiif*)malloc(sizeof(struct control_opt_noiif));

        return ctx;
}

static void destroy_all_memctx(struct ctx* ctx) {
        free(ctx->control_opt);
        free(ctx->control_opt_noiif);
        free(ctx);
}

int main(int argc, char *argv[]) {
        if (argc == 1) {
                show_general_help();
                return -1;
        }

        struct ctx* ctx = init_all_memctx();
        
        parse_opt(argc, argv, ctx);
        ethnetlink_main(ctx);

        destroy_all_memctx(ctx);
}