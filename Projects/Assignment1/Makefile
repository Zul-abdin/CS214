all:	main

main:	fileCompressor.out

%.out:	%.c
	gcc $< -o bin/$@

clean:
	rm -rf bin/fileCompressor

test:	bin/fileCompressor.out test0.txt test1 test2.txt

%.txt:	testing/testInput/%.txt
	cp testing/testInput/$@ testing/testOutput/$@
	./bin/fileCompressor.out -b testing/testOutput/$@
	./bin/fileCompressor.out -c testing/testOutput/$@ HuffmanCodebook
	./bin/fileCompressor.out -d testing/testOutput/$@.hcz HuffmanCodebook
	diff testing/testInput/$@ testing/testOutput/$@ | wc
	@rm -f testing/testOutput/*
	@rm -f HuffmanCodebook
	@echo "Done test: $@"

%:	testing/testInput/%
	cp -r testing/testInput/$@ testing/testOutput/$@
	./bin/fileCompressor.out -R -b testing/testOutput/$@
	./bin/fileCompressor.out -R -c testing/testOutput/$@ HuffmanCodebook
	./bin/fileCompressor.out -d testing/testOutput/$@.hcz HuffmanCodebook
	rm -f testing/testOutput/$@/*.hcz
	diff -r testing/testInput/$@ testing/testOutput/$@ | wc
	@rm -rf testing/testOutput/*
	@rm -f HuffmanCodebook
	@echo "Done test: $@"