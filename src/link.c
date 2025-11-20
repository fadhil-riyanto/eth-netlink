// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2025
 * Fadhil Riyanto <me@fadev.org> 
 */

#include "link.h"
#include "main.h"
#include <libmnl/libmnl.h>
#include <linux/if_link.h>
#include <linux/netlink.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <linux/rtnetlink.h> /* for RTM_GETLINK */
#include <linux/if_arp.h> /* for ARPHRD_NETROM */
#include "utils.h"

/**
 * \fn struct raw_buff eth_netlink_getphy(struct ctx* ctx)
 * \brief this func retrieve all interface information
 * using rnetlink, the struct that used is ifinfomsgh
 *
 * see also in
 * https://www.man7.org/linux/man-pages/man7/rtnetlink.7.html
 */

struct raw_buff eth_nl_link_dump_phy(struct ctx *ctx)
{
	struct mnl_socket *nl = ctx->nl;

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

	struct eth_netlink_cbdata cbdata = { .interface_counter = 1,
					     .ctx = ctx };

	while (ret > 0) {
		ret = mnl_cb_run(buf, ret, seq, portid, eth_nl_link_dump_cb,
				 &cbdata);

		if (ret <= 0)
			break;
		ret = mnl_socket_recvfrom(nl, buf, sizeof(buf));
	}
	// int n = 0;
	// do {
	// 	strcpy(rb->data + n, buf);
	// 	n = n + ret;
	// } while (ret != 0);
}

// int (*)(const struct nlattr *, void *)
int eth_nl_link_parse_nlattr(const struct nlattr *nlattr, void *data)
{
	struct eth_netlink_cbdata *cb = data;
	struct ctx *ctx = cb->ctx;

	cb = (struct eth_netlink_cbdata *)data;

	if (mnl_attr_get_type(nlattr) == IFLA_IFNAME) {
		char *phy = mnl_attr_get_payload(nlattr);
		printf("iface %d: %s\n", cb->interface_counter, phy);
		cb->interface_counter = cb->interface_counter + 1;
	}
	int nlattr_buf_size =
		mnl_attr_get_len(nlattr) - (int)sizeof(struct nlattr);

	if (mnl_attr_get_type(nlattr) == IFLA_TXQLEN) {
		printf("\tstatus: ");

		if (nlattr_buf_size == 1) {
			unsigned long txqlen = mnl_attr_get_u8(nlattr);
			printf("txqlen %lu", txqlen);
		}

		if (nlattr_buf_size == 2) {
			unsigned long txqlen = mnl_attr_get_u16(nlattr);
			printf("txqlen %lu", txqlen);
		}

		if (nlattr_buf_size == 4) {
			unsigned long txqlen = mnl_attr_get_u32(nlattr);
			printf("txqlen %lu", txqlen);
		}

		if (nlattr_buf_size == 8) {
			unsigned long txqlen = mnl_attr_get_u64(nlattr);
			printf("txqlen %lu", txqlen);
		}

		printf(", ");
	}

	if (mnl_attr_get_type(nlattr) == IFLA_OPERSTATE) {
		unsigned long state = mnl_attr_get_u8(nlattr);

		switch (state) {
		case 0:
			printf("iface_unknown");
			break;

		case 1:
			printf("iface_not_present");
			break;

		case 2:
			printf("iface_down");
			break;

		case 3:
			printf("iface_lowerlayereddown");
			break;

		case 4:
			printf("iface_testmode");
			break;

		case 5:
			printf("iface_dormant");
			break;

		case 6:
			printf("iface_up");
			break;
		}
		printf(", ");
	}

	if (mnl_attr_get_type(nlattr) == IFLA_LINKMODE) {
		unsigned long state = mnl_attr_get_u8(nlattr);
		printf("%s", (state == 1) ? "on_carrier" : "no_carrier");

		printf(", ");
	}

	if ((ctx->mode & ARG_COMMAND_DETAILS) == ARG_COMMAND_DETAILS &&
	    (mnl_attr_get_type(nlattr) == IFLA_NETNS_IMMUTABLE)) {
		
		unsigned long state = mnl_attr_get_u8(nlattr);

		print_bool("netns_immutable", state);
		// printf("%s", (state == 1) ? "CARRIER_ON" : "NO_CARRIER");
	}

	if (mnl_attr_get_type(nlattr) == IFLA_MTU) {
		unsigned int mtu = mnl_attr_get_u32(nlattr);
		printf("MTU %u", mtu);

		printf(", ");
	}

	if ((ctx->mode & ARG_COMMAND_DETAILS) == ARG_COMMAND_DETAILS &&
	    (mnl_attr_get_type(nlattr) == IFLA_MIN_MTU)) {
		
		unsigned int mtu = mnl_attr_get_u32(nlattr);
		printf("minMTU %u", mtu);
		printf(", ");
		// printf("%s", (state == 1) ? "CARRIER_ON" : "NO_CARRIER");
	}

	if ((ctx->mode & ARG_COMMAND_DETAILS) == ARG_COMMAND_DETAILS &&
	    (mnl_attr_get_type(nlattr) == IFLA_MAX_MTU)) {
		
		unsigned int mtu = mnl_attr_get_u32(nlattr);
		printf("maxMTU %u", mtu);
		printf(", ");
		// printf("%s", (state == 1) ? "CARRIER_ON" : "NO_CARRIER");
	}

	if ((ctx->mode & ARG_COMMAND_DETAILS) == ARG_COMMAND_DETAILS &&
	    (mnl_attr_get_type(nlattr) == IFLA_GROUP)) {
		
		unsigned int mtu = mnl_attr_get_u32(nlattr);
		printf("group %u", mtu);
		printf(", ");
		// printf("%s", (state == 1) ? "CARRIER_ON" : "NO_CARRIER");
	}

	return MNL_CB_OK;
}

/** \fn eth_netlink_phydev_cb(const struct nlmsghdr *nl, void *data)
 * \brief this func parse the message from kernel, the message that we parse is\n
 * ifinfomsg - for device information
 * nlattr - some shorts more detail about network interface
 */
int eth_nl_link_dump_cb(const struct nlmsghdr *nl, void *data)
{
	size_t ifinfomsgsz = sizeof(struct ifinfomsg);

	// struct eth_netlink_dbuf *dbuf = data;
	struct nlattr *buf = mnl_nlmsg_get_payload_offset(nl, ifinfomsgsz);

	mnl_attr_parse(nl, ifinfomsgsz, eth_nl_link_parse_nlattr, data);
	printf("\n");

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
