#include "main.h"
#include "netlink.h"
#include <libmnl/libmnl.h>
#include <linux/if_link.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <linux/if_arp.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "vt_hexdump.h"
#include <unistd.h>

static int dumped(const struct nlmsghdr* nl, void* data) {
        
        void* realptr = mnl_nlmsg_get_payload(nl);
        int len = mnl_nlmsg_get_payload_len(nl);

        char* needbuf = (char*)malloc(MNL_ALIGN(len));
        strncpy(needbuf, realptr, len);
        

        
        free(needbuf);

        return MNL_CB_OK;
}

int main()
{
	unsigned int seq, portid;
	struct ifinfomsg *ifinfomsgh;
	struct mnl_socket *mnl;
        int retq;

	char buf[MNL_SOCKET_DUMP_SIZE];
	struct nlmsghdr *nlh;
	
	nlh = mnl_nlmsg_put_header(buf);
	nlh->nlmsg_type = RTM_GETLINK;
	nlh->nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
	nlh->nlmsg_seq = seq = time(NULL);

        /**
         * test:
         * - https://docs.kernel.org/netlink/specs/rt-link.html#getlink
         * - https://docs.kernel.org/netlink/specs/rt-link.html#ifname-string
         * - https://docs.kernel.org/netlink/specs/rt-link.html#ext-mask-u32
         * - https://docs.kernel.org/netlink/specs/rt-link.html#target-netnsid-s32
         */

        mnl_attr_put_strz(nlh, IFLA_IFNAME, "br9");
        mnl_attr_put_u32(nlh, IFLA_EXT_MASK, RTEXT_FILTER_VF|RTEXT_FILTER_SKIP_STATS);
        mnl_attr_put_u32(nlh, IFLA_EXT_MASK, RTEXT_FILTER_VF|RTEXT_FILTER_SKIP_STATS);
        mnl_attr_put_u32(nlh, IFLA_IF_NETNSID, 923);


	mnl = mnl_socket_open(NETLINK_ROUTE);

        if (mnl_socket_bind(mnl, 0, MNL_SOCKET_AUTOPID) < 0) {
		perror("mnl_socket_bind");
		exit(EXIT_FAILURE);
	}

	portid = mnl_socket_get_portid(mnl);

	if (mnl_socket_sendto(mnl, nlh, nlh->nlmsg_len) < 0) {
		perror("mnl_socket_sendto");
                return -1;
	}

	retq = mnl_socket_recvfrom(mnl, buf, sizeof(buf));

        while (retq > 0) {
                // VT_HEXDUMP(buf, retq);
		retq = mnl_cb_run(buf, retq, seq, portid, dumped,
				 NULL);


		if (retq <= 0)
			break;
		retq = mnl_socket_recvfrom(mnl, buf, sizeof(buf));
	}

        

	mnl_socket_close(mnl);
}