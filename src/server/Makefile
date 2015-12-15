CC := gcc
CFLAGS := -c -O3 -W -Wall

objects := Main.o Common.o DbMysql.o Service.o ProcessFuncs.o

chat4lan: $(objects)
	$(CC) $(objects) -lmysqlclient -L /usr/lib/mysql/ -o chat4lan
Main.o: Main.c Common.h DbMysql.h Log.h ProcessFuncs.h
	$(CC) $(CFLAGS) Main.c
ProcessFuncs.o: ProcessFuncs.c ProcessFuncs.h
	$(CC) $(CFLAGS) ProcessFuncs.c
Common.o: Common.c Common.h Log.h
	$(CC) $(CFLAGS) Common.c
Service.o: Service.c Service.h Log.h Constants.h
	$(CC) $(CFLAGS) Service.c
DbMysql.o: DbMysql.c DbMysql.h Log.h
	$(CC) $(CFLAGS) DbMysql.c
clean:
	rm $(objects)
