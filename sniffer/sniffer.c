
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <asm/types.h>
#include <sys/socket.h>

#define MAXSIZE 2000
#define TCP 0x06
#define UDP 0x11
#define eth_len 14

void eth_header(unsigned char *);
void network_header(unsigned char *);
void transport_header(unsigned char *, int, int, int);
void data_header(unsigned char *, int, int, int);
//--------------------------------------------------------------
main()
{
    char buf[MAXSIZE + 1];
    unsigned char *eth, *network, *transport, *data;
    int len, network_len, transport_len, sockfd;
    int protocol, src_port, dst_port;

//---------------------------------------------	
        
    if ((sockfd = socket(PF_PACKET, SOCK_RAW, htons(0x0800))) < 0)
    {
	perror("socket");
	exit(1);
    }
    while (1)
    {
	if ((len = recvfrom(sockfd, buf, MAXSIZE, 0, NULL, NULL)) < 0)
	{
	    perror("recvfrom");
	    exit(1);
	}
	eth = buf;
	network = buf + eth_len;
	network_len = network[0];
	network_len &= 0x0f;
	network_len *= 4;
	transport  = network + network_len;
	if (network[9] == TCP) // TCP
	{
	    transport_len = 24;
	    protocol = TCP;
	}
	else if (network[9] == UDP) // UDP 
	{
	    transport_len = 8;
	    protocol = UDP;
	}
	
	src_port = transport[0];
	src_port <<= 8;
	src_port += transport[1];
	dst_port = transport[2];
	dst_port <<= 8;
	dst_port += transport[3];
	data = transport + transport_len;
	
	eth_header(eth);
	network_header(network);
	transport_header(transport, src_port, dst_port, protocol);
	data_header(data, len, network_len, transport_len);
	printf("\n\n--------------------------------\n\n");
    }
}	
//--------------------------------------------------------------
void eth_header(unsigned char *eth)
{
    printf("\ndatalink header: \n");
    printf("source MAC address: %02x:%02x:%02x:%02x:%02x:%02x\n", 
	eth[6], eth[7], eth[8], eth[9], eth[10], eth[11]);
    printf("destination MAC address: %02x:%02x:%02x:%02x:%02x:%02x\n", 
	eth[0], eth[1], eth[2], eth[3], eth[4], eth[5]);
    return;
}
//--------------------------------------------------------------
void network_header(unsigned char *network)
{
    printf("\nnetwork header: \n");
    printf("source IP address: %d.%d.%d.%d\n", 
    	network[12], network[13], network[14], network[15]);
    printf("destination IP address: %d.%d.%d.%d\n", 
	network[16], network[17], network[18], network[19]);
    return;
}
//--------------------------------------------------------------
void transport_header(unsigned char *transport, int src_port, int dst_port, int protocol)
{
    printf("\ntransport header: \n");
    if (protocol == TCP)
    {
	printf ("tcp source port: %d\n", src_port); 
	printf ("tcp destination port: %d\n", dst_port); 
    }
    else if (protocol == UDP) 
    {
	printf ("udp source port: %d\n", src_port); 
	printf ("udp destination port: %d\n", dst_port); 
    }
    return;
}	
//--------------------------------------------------------------
void data_header(unsigned char *data, int len, int network_len, int transport_len)
{ 	
    int i;
    printf ("\ndata: \n");
    for (i = 0; i < len - (transport_len + network_len + eth_len); i++)
    	if (data[i] <= 127 && data[i] >= 32)
	    printf("%c", data[i]);
	else if (data[i] == '\r')
	    printf("%c",'\r');
	else if (data[i] == '\n')
	    printf("%c",'\n');
	else if (data[i] == '\t')
	    printf("%c",'\t');
	else if (data[i] > 0)
	    printf("(%x)", data[i]);
    return;
}

