CC = gcc
CFLAGS =-Wall
DIR_PUBLIC = ../public
	
testClient:cliMain.o handleServer.o ../public/smsProtocal.o  ../public/myGetString.o
	$(CC) $(CFLAGS ) $^ $(CFLAGS) -o $@
cliMain.o:cliMain.c
	$(CC) $(CFLAGS ) -c $< -o cliMain.o
handleServer.o:handleServer.c
	$(CC) $(CFLAGS ) -c handleServer.c -o handleServer.o
../public/smsProtocal.o:../public/smsProtocal.c
	$(CC) $(CFLAGS ) -c ../public/smsProtocal.c -o ../public/smsProtocal.o
../public/myGetString.o:../public/myGetString.c
	$(CC) $(CFLAGS ) -c ../public/myGetString.c -o ../public/myGetString.o
clean:
	rm -rf *.o testClient ../public/*.o
