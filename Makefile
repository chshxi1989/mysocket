All:
	gcc -c myserver.c -o myserver.o
	gcc -c myclient.c -o myclient.o
	gcc myserver.o -o  myserver
	gcc myclient.o -o myclient
clean:
	rm myserver.o
	rm myserver
	rm myclient.o
	rm myclient
