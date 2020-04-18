all:	main

main:	bin-client/client.out bin-server/server.out

bin-server/%.out:	%.c | bin-server
	gcc $< -o $@ -lssl -lcrypto -lpthread

bin-client/%.out:	%.c | bin-client
	gcc $< -o $@ -lssl -lcrypto

bin-server:
	mkdir -p $@

bin-client:
	mkdir -p $@

%.out:	%.c
	gcc $< -o $@

clean:
	rm -rf bin-server
	rm -rf bin-client
