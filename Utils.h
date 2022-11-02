#include <stdint.h>

int get_ifindex_byifname(char *ifname);

char *getSrcIPaddr(char *nic_name);
void setSrcIPaddr(char *nic_name, uint8_t ip_arr[4]);
void setIPaddressArray(char *ipaddress, uint8_t ip_arr[4]);

void setSrcMACaddr(char *nic_name, uint8_t mac[6]);
