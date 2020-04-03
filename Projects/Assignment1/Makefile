all:	main

main:	fileCompressor.out

test:	bin/fileCompressor.out testOutput test0.txt test1 test2.txt
	@rm -rf testing/testOutput
	@echo "Done with all tests"

bin/%.out:	%.c | bin
	gcc $< -o $@

bin:
	mkdir -p $@

testOutput:
	mkdir -p testing/$@

%.out:	%.c
	gcc $< -o $@

clean:
	rm -f fileCompressor.out
	rm -rf bin
	rm -rf testing/testOutput

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