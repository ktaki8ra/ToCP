#include "Ethernet.h"

#include <stdlib.h>
#include <stdint.h>

EthernetHeader *NewEthernetHeader(uint8_t dst_mac[6], uint8_t src_mac[6], char *ethtype) {
	EthernetHeader *eth_header;
	eth_header = (EthernetHeader *)malloc(sizeof(EthernetHeader));

	eth_header->DstMacAddr[0] = dst_mac[0];
	eth_header->DstMacAddr[1] = dst_mac[1];
	eth_header->DstMacAddr[2] = dst_mac[2];
	eth_header->DstMacAddr[3] = dst_mac[3];
	eth_header->DstMacAddr[4] = dst_mac[4];
	eth_header->DstMacAddr[5] = dst_mac[5];

	eth_header->SrcMacAddr[0] = src_mac[0];
	eth_header->SrcMacAddr[1] = src_mac[1];
	eth_header->SrcMacAddr[2] = src_mac[2];
	eth_header->SrcMacAddr[3] = src_mac[3];
	eth_header->SrcMacAddr[4] = src_mac[4];
	eth_header->SrcMacAddr[5] = src_mac[5];

	eth_header->EthType[0] = 0x08;
	eth_header->EthType[1] = 0x06;

	return eth_header;
}

void FreeEthernetHeader(EthernetHeader *ethheader) {
	free(ethheader);
}

