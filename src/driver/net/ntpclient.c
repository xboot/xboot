/*
 * driver/net/ntpclient.c
 *
 * Copyright(c) 2007-2022 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <xboot.h>
#include <net/ntpclient.h>

/*
 *  7 6 5 4 3 2 1 0 7 6 5 4 3 2 1 0 7 6 5 4 3 2 1 0 7 6 5 4 3 2 1 0
 * +---+-----+-----+---------------+---------------+--------------+
 * |LI | VN  |Mode |    Stratum    |     Poll      |   Precision   | 0
 * +---+-----+-----+---------------+---------------+---------------+
 * |                          Root Delay                           | 1
 * +---------------------------------------------------------------+
 * |                       Root Dispersion                         | 2
 * +---------------------------------------------------------------+
 * |                     Reference Identifier                      | 3
 * +---------------------------------------------------------------+
 * |                    Reference Timestamp (64)                   | 4
 * +---------------------------------------------------------------+
 * |                    Originate Timestamp (64)                   | 6
 * +---------------------------------------------------------------+
 * |                     Receive Timestamp (64)                    | 8
 * +---------------------------------------------------------------+
 * |                     Transmit Timestamp (64)                   | 10
 * +---------------------------------------------------------------+
 * |                 Key Identifier (optional) (32)                | 12
 * +---------------------------------------------------------------+
 * |                 Message Digest (optional) (128)               | 13+
 * +---------------------------------------------------------------+
 */
struct ntp_packet {
	uint8_t flags;
	uint8_t stratum;
	uint8_t poll;
	uint8_t precision;
	uint32_t root_delay;
	uint32_t root_dispersion;
	uint8_t reference_id[4];
	uint32_t ref_ts_sec;
	uint32_t ref_ts_frac;
	uint32_t origin_ts_sec;
	uint32_t origin_ts_frac;
	uint32_t recv_ts_sec;
	uint32_t recv_ts_frac;
	uint32_t trans_ts_sec;
	uint32_t trans_ts_frac;
} __attribute__((__packed__));

static const char * ntp_hosts[] = {
	"ntp.aliyun.com",
	"time.windows.com",
	"time.asia.apple.com",
	"cn.ntp.org.cn",
	"cn.pool.ntp.org",
	"asia.pool.ntp.org",
};

static int __ntpclient_sync(struct net_t * net, const char * host)
{
	struct socket_connect_t * c;
	int ret = 0;

	c = net_connect(net, "udp", host, 123);
	if(c)
	{
		struct timeval tv;
		struct ntp_packet packet;
		memset(&packet, 0, sizeof(struct ntp_packet));
		packet.flags = (0x0 << 6) | (0x4 << 3) | (0x3 << 0);
		packet.stratum = 0;
		packet.poll = 4;
		packet.precision = -6;
		if(gettimeofday(&tv, NULL) == 0)
		{
			packet.trans_ts_sec = cpu_to_be32(tv.tv_sec + 2208988800ULL);
			packet.trans_ts_frac = cpu_to_be32((uint32_t)(((uint64_t)tv.tv_usec * ((uint64_t)1 << 31)) / 1000000.0));
		}
		if(net_write_timeout(c, &packet, sizeof(struct ntp_packet), 1000) == sizeof(struct ntp_packet))
		{
			if(net_read_timeout(c, &packet, sizeof(struct ntp_packet), 1000) == sizeof(struct ntp_packet))
			{
				packet.root_delay = cpu_to_be32(packet.root_delay);
				packet.root_dispersion = cpu_to_be32(packet.root_dispersion);
				packet.ref_ts_sec = cpu_to_be32(packet.ref_ts_sec);
				packet.ref_ts_frac = cpu_to_be32(packet.ref_ts_frac);
				packet.origin_ts_sec = cpu_to_be32(packet.origin_ts_sec);
				packet.origin_ts_frac = cpu_to_be32(packet.origin_ts_frac);
				packet.recv_ts_sec = cpu_to_be32(packet.recv_ts_sec);
				packet.recv_ts_frac = cpu_to_be32(packet.recv_ts_frac);
				packet.trans_ts_sec = cpu_to_be32(packet.trans_ts_sec);
				packet.trans_ts_frac = cpu_to_be32(packet.trans_ts_frac);
				tv.tv_sec = packet.recv_ts_sec - 2208988800ULL;
				tv.tv_usec = 0;
				settimeofday(&tv, NULL);
				ret = 1;
			}
		}
		net_close(c);
	}
	return ret;
}

int ntpclient_sync(struct net_t * net, const char * host)
{
	net = net ? net : search_first_net();
	if(net)
	{
		if(host)
			return __ntpclient_sync(net, host);
		else
		{
			for(int i = 0; i < ARRAY_SIZE(ntp_hosts); i++)
			{
				if(__ntpclient_sync(net, ntp_hosts[i]))
					return 1;
			}
		}
	}
	return 0;
}
