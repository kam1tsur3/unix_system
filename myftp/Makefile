all: myftpc.c myftpd.c myftp.h args.c err.c
	gcc -o myftpc myftpc.c args.c err.c
	gcc -o myftpd myftpd.c

clean:
	rm ./myftpc ./myftpd
