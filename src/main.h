#ifndef MAIN_H
#define MAIN_H

#define ETH_NETLINK_VERSION "0.01"

#define ARG_COMMAND_LIST_INTERFACE 0x1 << 0
#define ARG_COMMAND_INTERFACE 0x1 << 1
#define ARG_COMMAND_INFO 0x1 << 2
#define ARG_COMMAND_HELP 0x1 << 3

struct ctx* init_all_memctx();

#endif