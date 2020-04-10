all:	main

main:	bin/client.out bin/server.out

bin/%.out:	%.c | bin
	gcc $< -o $@

bin:
	mkdir -p $@

%.out:	%.c
	gcc $< -o $@

clean:
	rm -rf bin