all:	main

main:	fileCompressor.out

test:	bin/fileCompressor.out testOutput test0.txt test1 test2.txt test3 simplerecursivetest recursivetest1 recursivetest2 recursivetest3
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
	@cp testing/testInput/$@ testing/testOutput/$@
	@./bin/fileCompressor.out -b testing/testOutput/$@
	@./bin/fileCompressor.out -c testing/testOutput/$@ HuffmanCodebook
	@rm -f testing/testOutput/$@
	@./bin/fileCompressor.out -d testing/testOutput/$@.hcz HuffmanCodebook
	@diff testing/testInput/$@ testing/testOutput/$@ | wc
	@rm -f testing/testOutput/*
	@rm -f HuffmanCodebook
	@echo "Done test: $@"

%:	testing/testInput/%
	@cp -r testing/testInput/$@ testing/testOutput/$@
	@./bin/fileCompressor.out -R -b testing/testOutput/$@
	@./bin/fileCompressor.out -R -c testing/testOutput/$@ HuffmanCodebook
	@find testing/testOutput/$@ -type f -not -name '*.hcz' -delete
	@./bin/fileCompressor.out -R -d testing/testOutput/$@ HuffmanCodebook
	@find testing/testOutput -iname '*.hcz' -delete
	@diff -r testing/testInput/$@ testing/testOutput/$@ | wc
	@rm -rf testing/testOutput/*
	@rm -f HuffmanCodebook
	@echo "Done test: $@"