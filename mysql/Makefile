All:
	gcc -I/usr/local/mysql/include -c mysql-test.c -o mysql-test.o
	gcc -fPIC -L/usr/local/mysql/lib -lmysqlclient mysql-test.o -o mysql-test
