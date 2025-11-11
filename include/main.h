// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2025
 * Fadhil Riyanto <me@fadev.org> 
 */

#ifndef _MAIN_H_
#define _MAIN_H_

#define ETH_NETLINK_VERSION "0.01"

/**
 * the definition that hold getopt params, we use 64 bit unsigned.
 */
#define ARG_COMMAND_LIST_INTERFACE      0x1 << 0
#define ARG_COMMAND_INTERFACE           0x1 << 1
#define ARG_COMMAND_INFO                0x1 << 2
#define ARG_COMMAND_HELP                0x1 << 3
#define ARG_COMMAND_LICENSE             0x1 << 4

/**
 * \struct ctx
 *
 * this struct hold all metadata from start until it finishes
 * note that mode hold some important `bitfield` value
 *
 * bit 0 : `--list-interface` is set\n
 * bit 1 : has `--interface`, and `phydev` is set\n
 * bit 2 : has `--info` flag set
 * bit 3 : has `--help` flag set
 *
 */
struct ctx {
	unsigned long int mode;
	char *phydev;
	struct mnl_socket *nl;
};

/**
 * \struct raw_buff
 * \brief this is a TLV template for various buffer
 */
struct raw_buff {
	int length;
	char *data;
};

/**
 * \struct eth_netlink_cbdata
 * \brief this struct used as more parameter that being pass onto
 * `mnl_run_cb`, the function that uses this struct is `eth_netlink_phydev_cb` and 
 * `eth_netlink_parse_nlattr`
 * 
 */
struct eth_netlink_cbdata {
	int interface_counter;
};

struct ctx* init_all_memctx();
void parse_opt(int argc, char *argv[], struct ctx *ctx);
void destroy_all_memctx(struct ctx *ctx);

#endif