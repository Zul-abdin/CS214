all:
	gcc -lssl -lcrypto WTF.c -o WTF; gcc -lpthread -lssl -lcrypto WTFserver.c -o WTFserver

clean:
	rm -f WTF; rm -f WTFserver; rm -rf clientFolder; rm -rf serverFolder; rm -f WTFtest

test: client server
	gcc WTFtest.c -o WTFtest; gcc -lssl -lcrypto WTF.c -o ./clientFolder/WTF; gcc -lpthread -lssl -lcrypto WTFserver.c -o ./serverFolder/WTFserver

client:
	mkdir clientFolder

server:
	mkdir serverFolder
