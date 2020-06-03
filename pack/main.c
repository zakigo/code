#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
 
#include <netdb.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/in.h>
#include <netpacket/packet.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <net/ethernet.h>
 
 
#define BUFFER_SIZE 2048
 
int debug_level = 0;
 
#define _AUTHOR "Late Lee"
#define _VERSION_STR "1.0"
#define _DATE ""
 
// 默认打印error等级
enum
{
    MSG_ERROR = 0,
    MSG_WARNING,
    MSG_INFO,
    MSG_DEBUG,
    MSG_MSGDUMP,
    MSG_EXCESSIVE, 
};
 
void ll_printf(int level, const char *fmt, ...) __attribute__ ((format (printf, 2, 3)));
 
void ll_printf(int level, const char *fmt, ...)
{
    va_list ap;
 
    va_start(ap, fmt);
    if (debug_level >= level)
    {
#ifdef CONFIG_DEBUG_SYSLOG
        if (wpa_debug_syslog) {
            vsyslog(syslog_priority(level), fmt, ap);
        } else {
#endif /* CONFIG_DEBUG_SYSLOG */
        //debug_print_timestamp();
#ifdef CONFIG_DEBUG_FILE
        if (out_file) {
            vfprintf(out_file, fmt, ap);
            fprintf(out_file, "\n");
        } else {
#endif /* CONFIG_DEBUG_FILE */
        vprintf(fmt, ap);
        printf("\n");
#ifdef CONFIG_DEBUG_FILE
        }
#endif /* CONFIG_DEBUG_FILE */
#ifdef CONFIG_DEBUG_SYSLOG
        }
#endif /* CONFIG_DEBUG_SYSLOG */
    }
    va_end(ap);
}
 
void show_version(char* name)
{
    printf("%s by %s, version: %s\n", name, _AUTHOR, _VERSION_STR);
}
 
 
const char* my_opt = "hvd:i:";
 
void usage(char* name)
{
    show_version(name);
 
    printf("    -h,    short help\n");
    printf("    -v,    show version\n");
    printf("    -d,    debug level\n");
    printf("    -i,    ethernet device\n");
    
    exit(0);
}
 
void dump(const char *buffer, int len)
{
    int i, j, n;
    int line = 16;
    char c;
    unsigned char* buf = (unsigned char *)buffer;    // 必须是unsigned char类型
 
    n = len / line;
    if (len % line)
        n++;
 
    for (i=0; i<n; i++)
    {
        //printf("0x%08x: ", (unsigned int)(buf+i*line)); // linux ok
        printf("0x%8p: ", buf+i*line); // windows ok
        
        for (j=0; j<line; j++)
        {
            if ((i*line+j) < len)
                printf("%02x ", buf[i*line+j]);
            else
                printf("   ");
        }
 
        printf(" ");
        for (j=0; j<line && (i*line+j)<len; j++)
        {
            if ((i*line+j) < len)
            {
                c = buf[i*line+j];
                printf("%c", c >= ' ' && c < '~' ? c : '.');
            }
            else
                printf("   ");
        }
 
        printf("\n");
    }
}
 
int setPromiscMode(int socket, const char* eth, bool enable)
{
    // 混杂模式
    struct ifreq req;
    strcpy(req.ifr_name, eth);
 
    ioctl(socket, SIOCGIFFLAGS, &req);
    
    if (enable)
    {
        req.ifr_flags |= IFF_PROMISC;
    }
    else
    {
        req.ifr_flags &= ~IFF_PROMISC;
    }
    
    int ret = ioctl(socket, SIOCSIFFLAGS, &req);
    if (ret < 0)
    {
        perror("Set promisc mode failed:");
        return -1;
    }
 
    return 0;
}
 
int initSocket(const char* eth)
{
    int ret = 0;
    int fd = -1;
    
    if (eth == NULL)
    {
        return -1;
    }
    
    // 注意与下面绑定时协议一致
    fd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_IP)); // tocheck ETH_P_IP | 
    if (fd < 0)
    {
        perror("init socket failed:");
        return -1;
    }
 
    // 接收buffer大小
    int size = BUFFER_SIZE;
    ret = setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &size, sizeof(int));
    if (ret < 0)
    {
        goto end;
    }
 
    // 绑定网卡
    struct ifreq req;
    strcpy(req.ifr_name, eth);
    ioctl(fd, SIOCGIFINDEX, &req);
    
    struct sockaddr_ll addr;
    addr.sll_family = PF_PACKET;
    addr.sll_ifindex = req.ifr_ifindex;
    addr.sll_protocol = htons(ETH_P_IP); // 取决于此处协议 ETH_P_ALL  ETH_P_IP |ETH_P_ARP
    ret = bind(fd, (struct sockaddr *)&addr, sizeof(sockaddr_ll));
    if (ret < 0)
    {
        goto end;
    }
 
    // 混杂模式
    //setPromiscMode(fd, eth, true);
 
end:
    if (ret < 0)
    {
        close(fd);
        return ret;
    }
    else
    {
        return fd;
    }
}
 
int deintSocket(int socket)
{
    close(socket);
    return 0;
}
 
int showMacAddr(const struct ether_header *ethhead)
{
    printf("mac: [%02x:%02x:%02x:%02x:%02x:%02x] --> ",
                    ethhead->ether_shost[0], ethhead->ether_shost[1],
                    ethhead->ether_shost[2], ethhead->ether_shost[3],
                    ethhead->ether_shost[4], ethhead->ether_shost[5]);
 
    printf("mac: [%02x:%02x:%02x:%02x:%02x:%02x]\n",
                    ethhead->ether_dhost[0], ethhead->ether_dhost[1],
                    ethhead->ether_dhost[2], ethhead->ether_dhost[3],
                    ethhead->ether_dhost[4], ethhead->ether_dhost[5]);
 
    return 0;
}
 
int parseTcpHeader(const char* buffer)
{
    struct tcphdr* hdr = (struct tcphdr*)buffer;
    printf("port: [%d] --> [%d]\n", ntohs(hdr->th_sport), ntohs(hdr->th_dport));
    return 0;
}
 
int parseUdpHeader(const char* buffer)
{
    const struct udphdr* hdr = reinterpret_cast<const struct udphdr *>(buffer); // (struct udphdr*)buffer;
    // 长度字段与实际接收数据不符合，不知原因为何
    printf("port: [%d] --> [%d] [%d bytes]\n", ntohs(hdr->uh_sport), ntohs(hdr->uh_dport), ntohs(hdr->uh_ulen));
    return 0;
}
 
int parseIpHeader(const char* buffer)
{
    if (NULL == buffer)
    {
        return -1;
    }
    
    #if 0
    const struct ip* pstIP = reinterpret_cast<const struct ip *>(buffer); //(struct ip *)buffer;
    
    /* 协议类型、源IP地址、目的IP地址 */
    struct protoent* pstIpProto = getprotobynumber(pstIP->ip_p);
    if(NULL != pstIpProto)
    {
        printf("%s(%d) ", pstIpProto->p_name, pstIP->ip_p);
    }
    else
    {
        printf("%s(%d) ", "None", pstIP->ip_p);
    }
    printf("ip: [%s] --> ", inet_ntoa(pstIP->ip_src));
    printf("[%s]\n", inet_ntoa(pstIP->ip_dst));
    
    struct protoent* pstIpProto = getprotobynumber(hdr->protocol);
    if(NULL != pstIpProto)
    {
        printf("%s(%d) ", pstIpProto->p_name, hdr->protocol);
    }
    else
    {
        printf("%s(%d) ", "None", hdr->protocol);
    }    
    #endif
    
    const struct iphdr* hdr = (struct iphdr *)buffer;
 
    struct in_addr saddr;
    saddr.s_addr = hdr->saddr;
    printf("ip: [%s] --> ", inet_ntoa(saddr));
    saddr.s_addr = hdr->daddr;
    printf("[%s] [%d bytes] ", inet_ntoa(saddr), ntohs(hdr->tot_len));
    
    switch (hdr->protocol)
    {
        case IPPROTO_TCP:
            printf("TCP\n");
            parseTcpHeader(buffer+sizeof(iphdr));
            break;
        case IPPROTO_UDP:
            printf("UDP\n");
            parseUdpHeader(buffer+sizeof(iphdr));
            break;
        case IPPROTO_ICMP:
            printf("ICMP\n");
            break;
        case IPPROTO_IGMP:
            printf("IGMP\n");
            break;
        default:
            break;
    }
    #if 0
    if ( == IPPROTO_TCP)
    {
        printf("TCP\n");
        parseTcpHeader(buffer+sizeof(iphdr));
    }
    if (hdr->protocol == IPPROTO_UDP)
    {
        printf("UDP\n");
        parseUdpHeader(buffer+sizeof(iphdr));
    }
    if (hdr->protocol == IPPROTO_ICMP)
    {
        printf("ICMP\n");
    }
    if (hdr->protocol == IPPROTO_IGMP)
    {
        printf("IGMP\n");
    }
    #endif
    
    return 0;
}
 
int parsePakcet(const char* buffer)
{
    if (buffer == NULL)
    {
        return -1;
    }
    
    const struct ether_header *ethhead = reinterpret_cast<const struct ether_header *>(buffer); //(struct ether_header*)buffer;
 
    ///////////////////
    int type = ntohs(ethhead->ether_type);
 
    if (type == ETHERTYPE_ARP)
    {
        printf("ethnet type: ARP\n");
        showMacAddr(ethhead);
    }
    if (type == ETHERTYPE_IP)
    {
        //printf("ethnet type: IP\n");
        showMacAddr(ethhead);
        parseIpHeader(buffer+sizeof(struct ether_header));
    }
 
    
    //printf("-------------------------------------------\n");
    
    return 0;
}
 
int startCapture(int socket)
{
    int ret = 0;
    socklen_t len = sizeof(struct sockaddr);
    char buffer[BUFFER_SIZE] = {0};
    
    while (1)
    {
        memset(buffer, '\0', BUFFER_SIZE);
        ret = recvfrom(socket, buffer, BUFFER_SIZE, 0, NULL, &len);
        if (ret < 0)
        {
            continue;
        }
        //printf("--recv len: %d\n", ret);
        if (debug_level)
            dump(buffer, ret);
        parsePakcet(buffer);
 
        usleep(10*1000);
    }
    
    return 0;
}
 
int main(int argc, char* argv[])
{
    char eth[8] = "eth0";
    
    while(1)
    {
        int c = getopt(argc, argv, my_opt);
        ll_printf(8, "option char: %x %c\n", c, c);
        if (c < 0)
        {
            break;
        }
        switch(c)
        {
        case 'd':
                debug_level = atoi(optarg);
                //printf("debug level: %d\n", debug_level);
                break;
        case 'i':
                strncpy(eth, optarg, 8);
                break;
        case '?':
        case 'h':
        default:
                usage(argv[0]);
                break;
        }
    }
    
    printf("Start capture on %s\n", eth);
 
    int socket = initSocket(eth);
    if (socket < 0)
    {
        printf("Init socket failed.\n");
        return -1;
    }
    startCapture(socket);
    return 0;
}
