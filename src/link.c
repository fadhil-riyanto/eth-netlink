// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2025
 * Fadhil Riyanto <me@fadev.org> 
 */

#include "link.h"
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <linux/rtnetlink.h>            /* for RTM_GETLINK */
#include <linux/if_arp.h>               /* for ARPHRD_NETROM */

/**
 * \fn struct raw_buff eth_netlink_getphy(struct ctx* ctx)
 * \brief this func retrieve all interface information
 * using rnetlink, the struct that used is ifinfomsgh
 *
 * see also in
 * https://www.man7.org/linux/man-pages/man7/rtnetlink.7.html
 */

struct raw_buff eth_nl_link_dump_phy(struct mnl_socket *nl)
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

	struct eth_netlink_cbdata cbdata = { .interface_counter = 1 };

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
	struct eth_netlink_cbdata *cb;

	cb = (struct eth_netlink_cbdata *)data;

	if (mnl_attr_get_type(nlattr) == IFLA_IFNAME) {
		char *phy = mnl_attr_get_payload(nlattr);
		printf("iface %d: %s\n", cb->interface_counter, phy);
		cb->interface_counter = cb->interface_counter + 1;
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
