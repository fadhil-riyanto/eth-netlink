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
#include "internal.h"
#include "vt_hexdump.h"
#include "main.h"
#include "vt_bitfield.h"
#include <getopt.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <libmnl/libmnl.h>

/**
 * \defgroup ETH Netlink program
 * @{
 */

#define RSP_BUFFER_SIZE 0xffff

struct option opt[] = {
        {"list-interfaces", no_argument, 0, 0x0},
        {"interface", required_argument, 0, 0x1},
        {"info", no_argument, 0, 0x2},
        {"help", no_argument, 0, 0x3},
        {"i", no_argument, 0, 0x4},
        
        
        {0, 0, 0, 0}
};


/**
 * This func show an help
 */
void show_general_help(void) {
        printf("eth-netlink v\n" ETH_NETLINK_VERSION "(c) 2025 Fadhil Riyanto\n");
        printf("  build with gcc\n\n");
        printf("Print help / information:\n");
        printf("--list-interface\t\tshow an interface lists\n");
        printf("--interface --i\t\t\tset a target interface");
        
        
        
}

/**
 * \struct ctx 
 *
 * this struct hold all metadata from start until it finishes
 * note that mode hold some important `bitfield` value
 * 
 * bit 0 : `--list-interface` is set\n
 * bit 1 : has `--interface`, and `iif_value` is set\n
 * bit 2 : has `--info` flag set
 * bit 3 : has `--help` flag set
 * 
 */
struct ctx { 
        unsigned long int       mode;
        char                    *iif_value;
};

/**
 * \fn struct ctx* init_all_memctx()
 * \brief this func return allocated memory of struct ctx
 */
EXPORT_SYMBOL struct ctx* init_all_memctx() {
        struct ctx* ctx = (struct ctx*)malloc(sizeof(struct ctx));
        return ctx;
}

/**
 * \fn parse_opt(int argc, char *argv[], struct ctx* ctx)
 * \param argc arg counter from main
 * \param argv arg value from main
 * \param ctx a pointer into context struct
 * this func return allocated memory of struct ctx
 */
EXPORT_SYMBOL void parse_opt(int argc, char *argv[], struct ctx* ctx) {
        int optind = 1;
        int c;

        while (1) {
                c = getopt_long(argc, argv, "c:", opt, &optind);
                // printf("%c\n", c);
                if (c == -1)
                        break;

                switch (c) {
                        case 0x0:
                        ctx->mode = ctx->mode | ARG_COMMAND_LIST_INTERFACE;
                        break;

                        case 0x1:
                        ctx->mode = ctx->mode | ARG_COMMAND_INTERFACE;
                        break;

                        
                        case 0x2:
                        ctx->mode = ctx->mode | ARG_COMMAND_INFO;
                        break;
                        
                        case 0x3:
                        ctx->mode = ctx->mode | ARG_COMMAND_HELP;
                        break;
                        
                        case 0x4:
                        ctx->mode = ctx->mode | ARG_COMMAND_INTERFACE;
                        break;
                }
        }
}

/**
 * \fn destroy_all_memctx(struct ctx* ctx)
 * \brief this func destroy all preallocated memory of struct ctx
 */
EXPORT_SYMBOL void destroy_all_memctx(struct ctx* ctx) {
        free(ctx);
}

/**
 * \fn void eth_netlink_main(struct ctx* ctx)
 * \brief this func acs as second main function, this is where all netlink
 * message get composed
 */
EXPORT_SYMBOL void eth_netlink_main(struct ctx* ctx) {
        if (ctx->mode == ARG_COMMAND_LIST_INTERFACE) {
                printf("EXECUTING LIST INTERFACE");
        }

        if (ctx->mode == (ARG_COMMAND_INTERFACE | ARG_COMMAND_INFO)) {
                printf("EXECUTING info INTERFACE");
        }

        if (ctx->mode == (ARG_COMMAND_HELP)) {
                show_general_help();
        }

        
}

/**
 * static struct ctx* init_all_memctx()
 * this func return allocated memory of struct ctx
 */
EXPORT_SYMBOL int main(int argc, char *argv[]) {
        if (argc == 1) {
                show_general_help();
                return -1;
        }

        struct ctx* ctx = init_all_memctx();
        
        parse_opt(argc, argv, ctx);

        // VT_BITFIELD_DUMP(ctx->mode);
        eth_netlink_main(ctx);

        destroy_all_memctx(ctx);
}

/**
 * @}
 */
