// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2025
 * Fadhil Riyanto <me@fadev.org> 
 */


#ifndef _NETLINK_H_
#define _NETLINK_H_

#include <libmnl/libmnl.h>
#include "main.h"

void rtnl_open(struct ctx *ctx, int bus);
int rtnl_close(struct ctx *ctx);

#endif