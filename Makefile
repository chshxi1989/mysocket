All:
	gcc -c myserver.c -o myserver.o
	gcc myserver.o -o  myserver
	
	gcc -c myclient.c -o myclient.o
	gcc myclient.o -o myclient
	
	gcc -c myserver-multProcess.c -o myserver-multProcess.o
	gcc myserver-multProcess.o -o myserver-multProcess
	
	gcc -c myserver-multThread.c -o myserver-multThread.o
	gcc myserver-multThread.o -o myserver-multThread
clean:
	rm myserver.o
	rm myserver
	
	rm myclient.o
	rm myclient
	
	rm myserver-multProcess.o
	rm myserver-multProcess
	
	rm myserver-multThread.o
	rm myserver-multThread