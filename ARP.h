#include <stdint.h>

#include "Ethernet.h"
#include "Utils.h"

#define ARP_PACKET_LEN 42

typedef struct ArpRequest {
	uint8_t HardwareType[2];
	uint8_t ProtocolType[2];
	uint8_t HardwareLen[1];
	uint8_t ProtocolLen[1];
	uint8_t Opcode[2];
	uint8_t SenderHardwareAddr[6];
	uint8_t SenderProtocolAddr[4];
	uint8_t TargetHardwareAddr[6];
	uint8_t TargetProtocolAddr[4];
} ArpRequest;

typedef struct ArpReply {
	uint8_t HardwareType[2];
	uint8_t ProtocolType[2];
	uint8_t HardwareLen[1];
	uint8_t ProtocolLen[1];
	uint8_t Opcode[2];
	uint8_t SenderHardwareAddr[6];
	uint8_t SenderProtocolAddr[4];
	uint8_t TargetHardwareAddr[6];
	uint8_t TargetProtocolAddr[4];
} ArpReply;

typedef struct ARP {
	EthernetHeader *eth_header; // 14bytes
	ArpRequest *arp_req; // 28bytes
} ARP;


ArpRequest *NewArpRequest(char *network_if_name, char *dst_ip);

void FreeArpRequest(ArpRequest *arp_req);

void FreeArpReply(ArpReply *arp_rep);

ARP *NewARP(char *network_if_name, char *dst_ip);

void FreeARP(ARP *arp);

ArpReply *arpSend(int ifindex, ARP *arp);

ArpReply *parseArpPacket(uint8_t packet[80]);

void arp(char *network_if_name, char *dst_ip);
