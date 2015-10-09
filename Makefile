All:
	gcc -c myserver.c -o myserver.o
	gcc -c myclient.c -o myclient.o
	gcc -c myserver-multprocess.c -o myserver-multprocess.o
	gcc myserver.o -o  myserver
	gcc myclient.o -o myclient
	gcc myserver-multprocess.o -o myserver-multprocess
clean:
	rm myserver.o
	rm myserver
	rm myclient.o
	rm myclient
	rm myserver-multprocess.o
	rm myserver-multprocess
