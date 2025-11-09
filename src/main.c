// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2025
 * Fadhil Riyanto <fadhil.riyanto@gnuweeb.org>
 */
#include "main.h"
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

			{ 0, 0, 0, 0 } };

/**
 * This func show an help
 */
void show_general_help(void)
{
	printf("eth-netlink v\n" ETH_NETLINK_VERSION
	       "(c) 2025 Fadhil Riyanto\n");
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
 * \struct eth_netlink_dbuf
 * \brief this struct used as more parameter that being pass onto
 * `mnl_run_cb`, the function that uses this struct is `eth_netlink_phydev_cb` and 
 * `eth_netlink_parse_nlattr`
 * 
 */
struct eth_netlink_dbuf {
	int interface_counter;
};

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
 * \fn void rtnl_open(struct ctx *ctx, int bus)
 * \param ctx use regular struct ctx *ctx
 * \param bus see man(7) netlink
 */
void rtnl_open(struct ctx *ctx, int bus)
{
	ctx->nl = mnl_socket_open(bus);
	if (ctx->nl == NULL) {
		perror("mnl_socket_open");
	}

	if (mnl_socket_bind(ctx->nl, 0, MNL_SOCKET_AUTOPID) < 0) {
		perror("mnl_socket_bind");
		exit(EXIT_FAILURE);
	}
}

/**
 * \fn int rtnl_close(struct ctx *ctx)
 * \param ctx see struct ctx *ctx
 *
 * this return a mnl_socket_close statuscode, see 
 * https://manpages.debian.org/experimental/libmnl-doc/mnl_socket_close.3.en.html
 */
int rtnl_close(struct ctx *ctx)
{
	int retq = mnl_socket_close(ctx->nl);
	if (ctx->nl == NULL) {
		perror("mnl_socket_close");
		return -1;
	}

	return retq;
}

INTERNAL_SYMBOL void *__nlattr_next(void *buf)
{
	struct nlattr *cur_nlattr = (struct nlattr *)buf;
	unsigned int len = cur_nlattr->nla_len;
	return cur_nlattr + len;
}

INTERNAL_SYMBOL void *__nlattr_getpayload(struct nlattr *cur_nlattr)
{
	unsigned int len = cur_nlattr->nla_len;
	unsigned int required_byte = len - sizeof(struct nlattr);

	char *buf = (char *)malloc(required_byte);
	strncpy(buf, ((void *)cur_nlattr + (unsigned int)NLATTRHDR_SZ),
		required_byte);

	return buf;
}

// int (*)(const struct nlattr *, void *)
int eth_netlink_parse_nlattr(const struct nlattr *nlattr, void *data)
{
	if (mnl_attr_get_type(nlattr) == IFLA_IFNAME) {
		char *phy = mnl_attr_get_payload(nlattr);
		printf("iface: %s\n", phy);
		// free(phy);
	}
}

/** \fn eth_netlink_phydev_cb(const struct nlmsghdr *nl, void *data)
 * \brief this func parse the message from kernel, the message that we parse is\n
 * ifinfomsg - for device information
 * nlattr - some shorts more detail about network interface
 */
int eth_netlink_phydev_cb(const struct nlmsghdr *nl, void *data)
{
	size_t ifinfomsgsz = sizeof(struct ifinfomsg);

	struct eth_netlink_dbuf *dbuf = data;
	struct nlattr *buf = mnl_nlmsg_get_payload_offset(nl, ifinfomsgsz);
	mnl_attr_parse(nl, ifinfomsgsz, eth_netlink_parse_nlattr, NULL);

	// // /* keep raw pointer in mind */
	// // unsigned long x = ((void*)phydev_lists + 16);

	// unsigned int nlattr_n = real_nlmsg_len / sizeof(struct nlattr);

	// for (int i = 0; i < nlattr_n; i++) {
	// 	if (mnl_attr_get_type(buf) == IFLA_IFNAME) {
	// 		char *phy = mnl_attr_get_payload(buf);
	// 		printf("%s\n", phy);
	// 		// free(phy);
	// 	}

	// 	// VT_HEXDUMP(buf, 16 * 4);
	// 	buf = (void *)mnl_attr_next(buf);
	// }

	// printf("\nInterface name: %s",
	//        if_indextoname(phydev_lists->ifi_index, "wlan0"));
}

/**
 * \fn struct raw_buff eth_netlink_getphy(struct ctx* ctx)
 * \brief this func retrieve all interface information
 * using rnetlink, the struct that used is ifinfomsgh
 *
 * see also in
 * https://www.man7.org/linux/man-pages/man7/rtnetlink.7.html
 */

struct raw_buff eth_netlink_dump_phy(struct mnl_socket *nl)
{
	char buf[MNL_SOCKET_BUFFER_SIZE];

	struct nlmsghdr *nlh;
	struct ifinfomsg *ifinfomsgh;
	int ret;
	unsigned int seq, portid;

	struct raw_buff *rb =
		(struct raw_buff *)malloc(sizeof(struct raw_buff));
	memset(rb, 0, sizeof(struct raw_buff));
	rb->length = 0;
	rb->data = (char *)malloc(4096);

	nlh = mnl_nlmsg_put_header(buf);
	nlh->nlmsg_type = RTM_GETLINK;
	nlh->nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
	nlh->nlmsg_seq = seq = time(NULL);
	nlh->nlmsg_pid = 0;

	ifinfomsgh = mnl_nlmsg_put_extra_header(nlh, sizeof(struct ifinfomsg));
	ifinfomsgh->ifi_family = AF_PACKET;
	ifinfomsgh->ifi_type = ARPHRD_NETROM;
	ifinfomsgh->ifi_index = 0;
	ifinfomsgh->ifi_flags = 0;
	ifinfomsgh->ifi_change = 0;

	portid = mnl_socket_get_portid(nl);

	if (mnl_socket_sendto(nl, nlh, nlh->nlmsg_len) < 0) {
		perror("mnl_socket_sendto");
		exit(EXIT_FAILURE);
	}

	ret = mnl_socket_recvfrom(nl, buf, sizeof(buf));

	struct eth_netlink_dbuf dbuf;
	dbuf.interface_counter = 0;

	while (ret > 0) {
		ret = mnl_cb_run(buf, ret, seq, portid, eth_netlink_phydev_cb,
				 &dbuf);

		if (ret <= 0)
			break;
		ret = mnl_socket_recvfrom(nl, buf, sizeof(buf));
		dbuf.interface_counter = dbuf.interface_counter + 1;
	}
	// int n = 0;
	// do {
	// 	strcpy(rb->data + n, buf);
	// 	n = n + ret;
	// } while (ret != 0);
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

	/* first, set nl up */

	if (ctx->mode == ARG_COMMAND_LIST_INTERFACE) {
		printf("Gathering interface data... pleas wait\n\n");
		rtnl_open(ctx, NETLINK_ROUTE);

		/* dump ethernet interface */
		char *buf;

		struct raw_buff rb = eth_netlink_dump_phy(ctx->nl);
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
