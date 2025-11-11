// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2025
 * Fadhil Riyanto <me@fadev.org> 
 */

#include "netlink.h"
#include <stdio.h>
#include <stdlib.h>

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