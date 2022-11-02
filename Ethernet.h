#include <stdint.h>

typedef struct EthernetHeader {
	uint8_t DstMacAddr[6];
	uint8_t SrcMacAddr[6];
	uint8_t EthType[2];
} EthernetHeader;

EthernetHeader *NewEthernetHeader(uint8_t dst_mac[6], uint8_t src_mac[6], char *ethtype);

void FreeEthernetHeader(EthernetHeader *ethheader);
