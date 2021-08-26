CC = gcc
CFLAGS =-Wall -lpthread -lsqlite3
DIR_INCLUDE = ../include
DIR_PUBLIC = ../public
	
testServer:./serMain.o ./createSqlTables.o ./handleClient.o ./operationFunctions.o ../public/smsProtocal.o  ../public/myGetString.o
	$(CC) $(CFLAGS ) $^ $(CFLAGS) -o $@
$@:serMain.c
	$(CC) $(CFLAGS ) -c serMain.c -o $<
createSqlTables.o:createSqlTables.c
	$(CC) $(CFLAGS ) -c createSqlTables.c -o createSqlTables.o
handleClient.o:handleClient.c
	$(CC) $(CFLAGS ) -c handleClient.c -o handleClient.o
operationFunctions.o:operationFunctions.c
	$(CC) $(CFLAGS ) -c operationFunctions.c -o operationFunctions.o
../public/smsProtocal.o:../public/smsProtocal.c
	$(CC) $(CFLAGS ) -c ../public/smsProtocal.c -o ../public/smsProtocal.o
../public/myGetString.o:../public/myGetString.c
	$(CC) $(CFLAGS ) -c ../public/myGetString.c -o ../public/myGetString.o
clean:
	rm -rf *.o testServer ../public/*.o ../sqlite3/sms.db
