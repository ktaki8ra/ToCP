#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>

int get_ifindex_byifname(char *ifname) {
        int ifindex;
        if((ifindex = if_nametoindex(ifname)) == 0) {
                perror("Not found interface name");
		exit(1);
        }
        return ifindex;
}

char *getSrcIPaddr(char *nic_name) {
	int fd;
	struct ifreq ifr;

	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if(fd < 0) {
		perror("socket function Error in getSrcIPaddr");
		exit(1);
	}
	ifr.ifr_addr.sa_family = AF_INET;
	strncpy(ifr.ifr_name, nic_name, IFNAMSIZ-1);
	if(ioctl(fd, SIOCGIFADDR, &ifr) < 0) {
		perror("ioctl function Error in getSrcIPaddr");
		exit(1);
	}
	close(fd);

	return inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);
}

void setSrcIPaddr(char *nic_name, uint8_t ip_arr[4]) {
	char *ipaddress = getSrcIPaddr(nic_name);
	size_t len = strlen(ipaddress);
	int count=0;
	char buf[3] = {0, 0, 0};
	int ip_arr_counter = 0;
	for(int i=0; i<len+1; i++) {
		if(ipaddress[i] == '.' || ipaddress[i] == '\0') {
			switch (count) {
				case 1:
					ip_arr[ip_arr_counter] = (uint8_t)(buf[0] - '0');
					break;
				case 2:
					ip_arr[ip_arr_counter] = (uint8_t)((buf[0] - '0') * 10 + (buf[1] - '0'));
					break;
				case 3:
					ip_arr[ip_arr_counter] = (uint8_t)((buf[0] - '0') * 100 + (buf[1] - '0') * 10 + (buf[2] - '0'));
					break;
				default:
					printf("something Error\n");
			}
			count=0;
			ip_arr_counter++;
			buf[0] = 0;
			buf[1] = 0;
			buf[2] = 0;
		} else {
			buf[count] = ipaddress[i];
			count++;
		}
	}
	return;
}

void setIPaddressArray(char *ipaddress, uint8_t ip_arr[4]) {
	size_t len = strlen(ipaddress);
	int count=0;
	char buf[3] = {0, 0, 0};
	int ip_arr_counter = 0;
	for(int i=0; i<len+1; i++) {
		if(ipaddress[i] == '.' || ipaddress[i] == '\0') {
			switch (count) {
				case 1:
					ip_arr[ip_arr_counter] = (uint8_t)(buf[0] - '0');
					break;
				case 2:
					ip_arr[ip_arr_counter] = (uint8_t)((buf[0] - '0') * 10 + (buf[1] - '0'));
					break;
				case 3:
					ip_arr[ip_arr_counter] = (uint8_t)((buf[0] - '0') * 100 + (buf[1] - '0') * 10 + (buf[2] - '0'));
					break;
				default:
					printf("something Error\n");
			}
			count=0;
			ip_arr_counter++;
			buf[0] = 0;
			buf[1] = 0;
			buf[2] = 0;
		} else {
			buf[count] = ipaddress[i];
			count++;
		}
	}
	return;
}

void setSrcMACaddr(char *nic_name, uint8_t mac[6]) {
	struct ifreq ifr;
	int sock;
	sock=socket(AF_INET, SOCK_DGRAM, 0);
	if(sock < 0) {
		perror("socket function Error in setSrcMACaddr");
	}
	ifr.ifr_addr.sa_family = AF_INET;
	strcpy(ifr.ifr_name, nic_name);
	if(ioctl(sock, SIOCGIFHWADDR, &ifr) < 0) {
		perror("ioctl function Error in setSrcMACaddr");
	}
	memcpy(mac, ifr.ifr_hwaddr.sa_data, 6);
	close(sock);
	return;
}
