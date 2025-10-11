// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2025
 * Fadhil Riyanto <fadhil.riyanto@gnuweeb.org>
 */

#include <stdio.h>
// #include "vt_hexdump.h"
#include <getopt.h>
#include <stdlib.h>

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

static void ethnetlink_main(struct ctx* ctx) {
        if (ctx->carry_spesific_interface == 0) {
                if (ctx->control_opt_noiif->list_interfaces == 1) {
                        printf("executing list interface");
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