ARP_EXE_PROGRAM = arp.exe

arp: main.c Utils.o Ethernet.o ARP.o
	gcc main.c Utils.o Ethernet.o ARP.o -o $(ARP_EXE_PROGRAM)
Utils.o:
	gcc -c Utils.c
Ethernet.o: 
	gcc -c Ethernet.c
ARP.o:
	gcc -c ARP.c
clean:
	rm -f *.o
