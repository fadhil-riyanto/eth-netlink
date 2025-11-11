// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2025
 * Fadhil Riyanto <me@fadev.org> 
 */
 
#include "main.h"
#include "netlink.h"
#include "link.h"

#include "../submodule/log.c/src/log.h"
#include "internal.h"
#include "vt_bitfield.h"
#include "vt_hexdump.h"
#include <asm/types.h>
#include <errno.h>
#include <getopt.h>
#include <libmnl/libmnl.h>
#include <linux/genetlink.h>
#include <linux/if_arp.h>
#include <linux/netdevice.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <linux/if_link.h>
// #include <net/if.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

/**
 * \defgroup ETH Netlink program
 * @{
 */

/* 2^16 size */
#define RSP_BUFFER_SIZE 0xffff
#define NLATTRHDR_SZ sizeof(struct nlattr)

struct option opt[] = { { "list-interfaces", no_argument, 0, 0x0 },
			{ "interface", required_argument, 0, 0x1 },
			{ "info", no_argument, 0, 0x2 },
			{ "help", no_argument, 0, 0x3 },
			{ "i", no_argument, 0, 0x4 },
			{ "license", no_argument, 0, 0x5 },

			{ 0, 0, 0, 0 } };

/**
 * This func show an help
 */
void show_general_help(void)
{
	printf("eth-netlink v" ETH_NETLINK_VERSION
	       " (c) 2025 Fadhil Riyanto\n");
	printf("  build with gcc %d.%d.%d\n\n", __GNUC__, __GNUC_MINOR__,
	       __GNUC_PATCHLEVEL__);
	printf("Print help / information:\n");
	printf("--list-interface\t\tshow an interface lists\n");
	printf("--interface|--i\t\t\tset a target interface\n");
	printf("--license\t\t\tDisplay the license/warranty terms of this program.");
}

void show_license(void)
{
	printf("Copyright 2025 Fadhil Riyanto (me@fadev.org)\n\n");

	printf("This program is free software; you can redistribute it and/or modify it\n");
	printf("under the terms of the GNU General Public License as published by the Free\n");
	printf("Software Foundation; either version 2 of the License, or (at your option)\n");
	printf("any later version.\n\n");

	printf("This program is distributed in the hope that it will be useful, but\n");
	printf("WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY\n");
	printf("or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License\n");
	printf("for more details.\n\n");

	printf("You should have received a copy of the GNU General Public License\n");
	printf("along with this program.  If not, see <https://www.gnu.org/licenses/>.\n");
}




/**
 * \fn struct ctx* init_all_memctx()
 * \brief this func return allocated memory of struct ctx
 */
EXPORT_SYMBOL struct ctx *init_all_memctx()
{
	struct ctx *ctx = (struct ctx *)malloc(sizeof(struct ctx));
	memset(ctx, 0, sizeof(struct ctx));
	return ctx;
}

/**
 * \fn parse_opt(int argc, char *argv[], struct ctx* ctx)
 * \param argc arg counter from main
 * \param argv arg value from main
 * \param ctx a pointer into context struct
 * this func return allocated memory of struct ctx
 */
EXPORT_SYMBOL void parse_opt(int argc, char *argv[], struct ctx *ctx)
{
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
			ctx->phydev = optarg;
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

		case 0x5:
			ctx->mode = ctx->mode | ARG_COMMAND_LICENSE;
			break;
		}
	}
}

/**
 * \fn destroy_all_memctx(struct ctx* ctx)
 * \brief this func destroy all preallocated memory of struct ctx
 */
EXPORT_SYMBOL void destroy_all_memctx(struct ctx *ctx)
{
	free(ctx);
}

/**
 * \fn void eth_netlink_main(struct ctx* ctx)
 * \brief this func acs as second main function, this is where all netlink
 * message get composed
 */
EXPORT_SYMBOL int eth_netlink_main(struct ctx *ctx)
{
	int retq = 0;

	if (ctx->mode == (ARG_COMMAND_HELP)) {
		show_general_help();
		return retq;
	}

	if (ctx->mode == ARG_COMMAND_LICENSE) {
		show_license();
	}

	/* first, set nl up */

	if (ctx->mode == ARG_COMMAND_LIST_INTERFACE) {
		printf("Gathering interface data... please wait\n\n");
		rtnl_open(ctx, NETLINK_ROUTE);

		/* dump ethernet interface */
		char *buf;

		struct raw_buff rb = eth_nl_link_dump_phy(ctx->nl);
		rtnl_close(ctx);
	}

	if (ctx->mode == (ARG_COMMAND_INTERFACE | ARG_COMMAND_INFO)) {
		log_info("executing info on interface %s", ctx->phydev);
	}

	return retq;
}

/**
 * static struct ctx* init_all_memctx()
 * this func return allocated memory of struct ctx
 */
EXPORT_SYMBOL int main(int argc, char *argv[])
{
	int retq = 0;
	if (argc == 1) {
		show_general_help();
		return -1;
	}

	struct ctx *ctx = init_all_memctx();

	parse_opt(argc, argv, ctx);

	// VT_BITFIELD_DUMP(ctx->mode);
	retq = eth_netlink_main(ctx);

	destroy_all_memctx(ctx);
}

/**
 * @}
 */
