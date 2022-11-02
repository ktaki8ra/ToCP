# ToCP
Toys of Communication Protocol
Environment: `20.04.2 LTS`

## ARP
```c
// main.c
#include <stdio.h>
#include "ARP.h"
int main() {
        arp("ens4", "192.168.20.12");
        return 0;
}
```
```
$ make arp
$ ./arp.exe
```
