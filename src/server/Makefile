chat4lan: Main.o Common.o DbMysql.o Service.o
	gcc Main.o Common.o DbMysql.o Service.o -lmysqlclient -L /usr/lib/mysql/ -o chat4lan
Main.o: Main.c Common.h DbMysql.h Log.h
	gcc -c Main.c
Common.o: Common.c Common.h Log.h
	gcc -c Common.c
Service.o: Service.c Service.h Log.h Constants.h
	gcc -c Service.c
DbMysql.o: DbMysql.c DbMysql.h Log.h
	gcc -c DbMysql.c
clean:
	rm *.o
