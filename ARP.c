#include "ARP.h"

#include "Utils.h"

#include <stdlib.h>
#include <stdint.h>

#include <stdio.h>
#include <string.h>

#include <unistd.h>

#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <linux/ip.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/ether.h>

ArpRequest *NewArpRequest(char *network_if_name, char *dst_ip) {

	ArpRequest *arp_req;
	arp_req = (ArpRequest *)malloc(sizeof(ArpRequest));

	arp_req->HardwareType[0] = 0x00;
	arp_req->HardwareType[1] = 0x01;

	arp_req->ProtocolType[0] = 0x08;
	arp_req->ProtocolType[1] = 0x00;

	arp_req->HardwareLen[0] = 0x06;

	arp_req->ProtocolLen[0] = 0x04;

	arp_req->Opcode[0] = 0x00;
	arp_req->Opcode[1] = 0x01;

	// 自分のMacアドレスを取得する処理
	setSrcMACaddr(network_if_name, arp_req->SenderHardwareAddr);

	// 自分のIPアドレス、CIDRを取得する処理
	setSrcIPaddr(network_if_name, arp_req->SenderProtocolAddr);

	// 相手のMacアドレス(ブロードキャスト)
	arp_req->TargetHardwareAddr[0] = 0x00;
	arp_req->TargetHardwareAddr[1] = 0x00;
	arp_req->TargetHardwareAddr[2] = 0x00;
	arp_req->TargetHardwareAddr[3] = 0x00;
	arp_req->TargetHardwareAddr[4] = 0x00;
	arp_req->TargetHardwareAddr[5] = 0x00;

	// 相手のIPアドレス
	setIPaddressArray(dst_ip, arp_req->TargetProtocolAddr);

	return arp_req;
}

void FreeArpRequest(ArpRequest *arp_req) {
	free(arp_req);
}

void FreeArpReply(ArpReply *arp_rep) {
	free(arp_rep);
}

ARP *NewARP(char *network_if_name, char *dst_ip) {
	EthernetHeader *eth_header;
	eth_header = (EthernetHeader *)malloc(sizeof(EthernetHeader));

	uint8_t src_mac[6] = {0};
	setSrcMACaddr(network_if_name, src_mac);
	uint8_t dst_mac[6] = {0xff};
	eth_header = NewEthernetHeader(dst_mac, src_mac, "ARP");

	ArpRequest *arp_request;
	arp_request = (ArpRequest *)malloc(sizeof(ArpRequest));
	arp_request = NewArpRequest(network_if_name, dst_ip);

	ARP *arp;
	arp = (ARP *)malloc(sizeof(ARP));
	arp->eth_header = eth_header;
	arp->arp_req = arp_request;

	return arp;
}

void FreeARP(ARP *arp) {
	FreeEthernetHeader(arp->eth_header);
	FreeArpRequest(arp->arp_req);
	free(arp);
}

ArpReply *arpSend(int ifindex, ARP *arp) {
	uint8_t packet[ARP_PACKET_LEN] = {
		arp->eth_header->DstMacAddr[0], arp->eth_header->DstMacAddr[1], arp->eth_header->DstMacAddr[2], arp->eth_header->DstMacAddr[3], arp->eth_header->DstMacAddr[4], arp->eth_header->DstMacAddr[5],
		arp->eth_header->SrcMacAddr[0], arp->eth_header->SrcMacAddr[1], arp->eth_header->SrcMacAddr[2], arp->eth_header->SrcMacAddr[3], arp->eth_header->SrcMacAddr[4], arp->eth_header->SrcMacAddr[5],
		arp->eth_header->EthType[0], arp->eth_header->EthType[1], 
		arp->arp_req->HardwareType[0], arp->arp_req->HardwareType[1], 
		arp->arp_req->ProtocolType[0], arp->arp_req->ProtocolType[1],
		arp->arp_req->HardwareLen[0],
		arp->arp_req->ProtocolLen[0],
		arp->arp_req->Opcode[0], arp->arp_req->Opcode[1],
		arp->arp_req->SenderHardwareAddr[0], arp->arp_req->SenderHardwareAddr[1], arp->arp_req->SenderHardwareAddr[2], arp->arp_req->SenderHardwareAddr[3], arp->arp_req->SenderHardwareAddr[4], arp->arp_req->SenderHardwareAddr[5],
		arp->arp_req->SenderProtocolAddr[0], arp->arp_req->SenderProtocolAddr[1], arp->arp_req->SenderProtocolAddr[2], arp->arp_req->SenderProtocolAddr[3],
		arp->arp_req->TargetHardwareAddr[0], arp->arp_req->TargetHardwareAddr[1], arp->arp_req->TargetHardwareAddr[2], arp->arp_req->TargetHardwareAddr[3], arp->arp_req->TargetHardwareAddr[4], arp->arp_req->TargetHardwareAddr[5],
		arp->arp_req->TargetProtocolAddr[0], arp->arp_req->TargetProtocolAddr[1], arp->arp_req->TargetProtocolAddr[2], arp->arp_req->TargetProtocolAddr[3]
	};

        int sendfd;
        struct sockaddr_ll addr;

        uint8_t buf[80];

        memset(&addr, 0x00, sizeof(addr));
        //addr.sll_protocol = htons(ETH_P_ARP);
        addr.sll_protocol = ETH_P_ARP;
        addr.sll_ifindex  = ifindex;
        //addr.sll_hatype   = htons(ARPHRD_ETHER);
        addr.sll_hatype   = ARPHRD_ETHER;

        //sendfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
        sendfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ARP));
        //sendfd = socket(AF_PACKET, SOCK_RAW, ETH_P_ALL);
        if(sendfd < 0) {
                perror("create socket failed\n");
                exit(1);
        }

        if(sendto(sendfd, packet, ARP_PACKET_LEN, 0, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
                perror("send failed\n");
                exit(1);
        }

        ssize_t size;
        if((size = recvfrom(sendfd, buf, sizeof(buf), 0, NULL , NULL)) < 0) {
                perror("receive failed\n");
                exit(1);
        }
        close(sendfd);

        // check ethernet type is ARP and buffer is truely ARP reply.
        if((buf[12] == 0x08 && buf[13] == 0x06) && (buf[20] == 0x00 && buf[21] == 0x02)) {
		return parseArpPacket(buf);
        } else {
		printf("MAC address: NOT FOUND\n");
		exit(1);
	}
}

ArpReply *parseArpPacket(uint8_t packet[80]) {
	ArpReply *arpReply;
	arpReply = (ArpReply *)malloc(sizeof(ArpReply));

	arpReply->HardwareType[0] = packet[14];
	arpReply->HardwareType[1] = packet[15];

	arpReply->ProtocolType[0] = packet[16];
	arpReply->ProtocolType[1] = packet[17];

	arpReply->HardwareLen[0] = packet[18];

	arpReply->ProtocolLen[0] = packet[19];

	arpReply->Opcode[0] = packet[20];
	arpReply->Opcode[1] = packet[21];

	arpReply->SenderHardwareAddr[0] = packet[22];
	arpReply->SenderHardwareAddr[1] = packet[23];
	arpReply->SenderHardwareAddr[2] = packet[24];
	arpReply->SenderHardwareAddr[3] = packet[25];
	arpReply->SenderHardwareAddr[4] = packet[26];
	arpReply->SenderHardwareAddr[5] = packet[27];

	arpReply->SenderProtocolAddr[0] = packet[28];
	arpReply->SenderProtocolAddr[1] = packet[29];
	arpReply->SenderProtocolAddr[2] = packet[30];
	arpReply->SenderProtocolAddr[3] = packet[31];

	arpReply->TargetHardwareAddr[0] = packet[32];
	arpReply->TargetHardwareAddr[1] = packet[33];
	arpReply->TargetHardwareAddr[2] = packet[34];
	arpReply->TargetHardwareAddr[3] = packet[35];
	arpReply->TargetHardwareAddr[4] = packet[36];
	arpReply->TargetHardwareAddr[5] = packet[37];

	arpReply->TargetProtocolAddr[0] = packet[38];
	arpReply->TargetProtocolAddr[1] = packet[39];
	arpReply->TargetProtocolAddr[2] = packet[40];
	arpReply->TargetProtocolAddr[3] = packet[41];

	return arpReply;
}

void arp(char *network_if_name, char *dst_ip) {
	int ifindex;
	ARP *arp = NewARP(network_if_name, dst_ip);
	ifindex = get_ifindex_byifname(network_if_name);
	ArpReply *arp_rep = arpSend(ifindex, arp);
	printf("IP address:  %d.%d.%d.%d\n",
			arp_rep->SenderProtocolAddr[0],
			arp_rep->SenderProtocolAddr[1],
			arp_rep->SenderProtocolAddr[2],
			arp_rep->SenderProtocolAddr[3]
	);
	printf("MAC address: %02x:%02x:%02x:%02x:%02x:%02x\n",
			arp_rep->SenderHardwareAddr[0],
			arp_rep->SenderHardwareAddr[1],
			arp_rep->SenderHardwareAddr[2],
			arp_rep->SenderHardwareAddr[3],
			arp_rep->SenderHardwareAddr[4],
			arp_rep->SenderHardwareAddr[5]
	);
	FreeArpReply(arp_rep);
	FreeARP(arp);
}
