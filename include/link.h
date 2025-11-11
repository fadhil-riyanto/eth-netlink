// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2025
 * Fadhil Riyanto <me@fadev.org> 
 */


#ifndef _LINK_H_
#define _LINK_H_

#include <libmnl/libmnl.h>
#include "main.h"

struct raw_buff eth_nl_link_dump_phy(struct mnl_socket *nl);
int eth_nl_link_parse_nlattr(const struct nlattr *nlattr, void *data);
int eth_nl_link_dump_cb(const struct nlmsghdr *nl, void *data);

#endif