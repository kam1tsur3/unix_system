all: mydhcpc.c mydhcpd.c mydhcp.h list.c
	gcc -o mydhcpc mydhcpc.c list.c
	gcc -o mydhcpd mydhcpd.c list.c

clean:
	rm ./mydhcpc ./mydhcpd
